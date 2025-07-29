# Import packages
import os
import argparse
import cv2
import numpy as np
import sys
import time
import zmq  # Replacing imagezmq with zmq
import importlib.util
from can import MCP2515  # Import the CAN controller class

# Initialize CAN controller
can_controller = MCP2515(bus=0, device=0, speed_hz=10000000)
if not can_controller.initialize(speed='1000kBPS'):
    print("Failed to initialize CAN controller")
    sys.exit(1)

# Define CAN message IDs and data values
CAN_ID = 0x104
SPEED_30 = 1
SPEED_40 = 2
SPEED_50 = 3
SPEED_60 = 4
SPEED_70 = 5
SPEED_80 = 6
SPEED_90 = 7
SPEED_100 = 8
SPEED_120 = 9
STOP_SIGN = 10
RED_LIGHT = 11
GREEN_LIGHT = 12
YELLOW_LIGHT = 13

# Track previous detection to only send on change
prev_detection = None

# Define and parse input arguments
parser = argparse.ArgumentParser()
parser.add_argument('--modeldir', help='Folder the .tflite file is located in', required=True)
parser.add_argument('--graph', help='Name of the .tflite file, if different than detect.tflite', default='detect.tflite')
parser.add_argument('--labels', help='Name of the labelmap file, if different than labelmap.txt', default='labelmap.txt')
parser.add_argument('--threshold', help='Minimum confidence threshold for displaying detected objects', default=0.5)
parser.add_argument('--resolution', help='Desired webcam resolution in WxH. If the webcam does not support the resolution entered, errors may occur.', default='1280x720')
parser.add_argument('--edgetpu', help='Use Coral Edge TPU Accelerator to speed up detection', action='store_true')

args = parser.parse_args()

MODEL_NAME = args.modeldir
GRAPH_NAME = args.graph
LABELMAP_NAME = args.labels
min_conf_threshold = float(args.threshold)
resW, resH = args.resolution.split('x')
imW, imH = int(resW), int(resH)
use_TPU = args.edgetpu

# Import TensorFlow libraries
pkg = importlib.util.find_spec('tflite_runtime')
if pkg:
    from tflite_runtime.interpreter import Interpreter
    if use_TPU:
        from tflite_runtime.interpreter import load_delegate
else:
    from tensorflow.lite.python.interpreter import Interpreter
    if use_TPU:
        from tensorflow.lite.python.interpreter import load_delegate

# If using Edge TPU, assign filename for Edge TPU model
if use_TPU:
    if GRAPH_NAME == 'detect.tflite':
        GRAPH_NAME = 'edgetpu.tflite'

# Get path to current working directory
CWD_PATH = os.getcwd()

# Path to .tflite file, which contains the model that is used for object detection
PATH_TO_CKPT = os.path.join(CWD_PATH, MODEL_NAME, GRAPH_NAME)

# Path to label map file
PATH_TO_LABELS = os.path.join(CWD_PATH, MODEL_NAME, LABELMAP_NAME)

# Load the label map
with open(PATH_TO_LABELS, 'r') as f:
    labels = [line.strip() for line in f.readlines()]

# Remove '???' label if present
if labels[0] == '???':
    del(labels[0])

# Load the Tensorflow Lite model
if use_TPU:
    interpreter = Interpreter(model_path=PATH_TO_CKPT, experimental_delegates=[load_delegate('libedgetpu.so.1.0')])
else:
    interpreter = Interpreter(model_path=PATH_TO_CKPT)

interpreter.allocate_tensors()

# Get model details
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()
height = input_details[0]['shape'][1]
width = input_details[0]['shape'][2]

floating_model = (input_details[0]['dtype'] == np.float32)

input_mean = 127.5
input_std = 127.5

# Check output layer name to determine if this model was created with TF2 or TF1
outname = output_details[0]['name']
if 'StatefulPartitionedCall' in outname:  # TF2 model
    boxes_idx, classes_idx, scores_idx = 1, 3, 0
else:  # TF1 model
    boxes_idx, classes_idx, scores_idx = 0, 1, 2

# Initialize frame rate calculation
frame_rate_calc = 1
freq = cv2.getTickFrequency()

# Initialize ZeroMQ subscriber (same as SUB1.py)
context = zmq.Context()
socket = context.socket(zmq.SUB)
socket.setsockopt(zmq.CONFLATE, 1)  # Keep only last message
socket.connect("tcp://127.0.0.1:5555")
socket.setsockopt_string(zmq.SUBSCRIBE, '')

def send_can_message(speed=None, direction=None):
    """Send CAN message with speed and/or direction information"""
    data = [0, 0]  # Initialize with zeros
    
    if speed is not None:
        data[0] = speed
        
    if direction is not None:
        data[1] = direction
    
    # Only send if at least one value is set
    if speed is not None or direction is not None:
        if can_controller.send_message(CAN_ID, data):
            print(f"Sent CAN message: ID=0x{CAN_ID:03X}, Data={[hex(x) for x in data]}")
        else:
            print("Failed to send CAN message (no buffers available)")

try:
    while True:
        # Start timer (for calculating frame rate)
        t1 = cv2.getTickCount()

        try:
            # Receive frame from publisher (same as SUB1.py)
            frame_data = socket.recv(zmq.NOBLOCK)
            
            # Convert bytes to numpy array and decode
            nparr = np.frombuffer(frame_data, np.uint8)
            frame = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
            
            if frame is None:
                continue
                
            # Acquire frame and resize to expected shape [1xHxWx3]
            frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            frame_resized = cv2.resize(frame_rgb, (width, height))
            input_data = np.expand_dims(frame_resized, axis=0)

            # Normalize pixel values if using a floating model
            if floating_model:
                input_data = (np.float32(input_data) - input_mean) / input_std

            # Perform the actual detection by running the model with the image as input
            interpreter.set_tensor(input_details[0]['index'], input_data)
            interpreter.invoke()

            # Retrieve detection results
            boxes = interpreter.get_tensor(output_details[boxes_idx]['index'])[0]  # Bounding box coordinates
            classes = interpreter.get_tensor(output_details[classes_idx]['index'])[0]  # Class indices
            scores = interpreter.get_tensor(output_details[scores_idx]['index'])[0]  # Confidence scores

            current_detection = None
            
            # Loop over all detections and process them
            for i in range(len(scores)):
                if (scores[i] > min_conf_threshold) and (scores[i] <= 1.0):
                    object_name = labels[int(classes[i])]
                    confidence = int(scores[i] * 100)
                    print(f"Detected: {object_name} - Confidence: {confidence}%")
                    
                    # Determine what we detected
                    if "30" in object_name or "speed limit 30" in object_name.lower():
                        current_detection = "speed30"
                    elif "40" in object_name or "speed limit 40" in object_name.lower():
                        current_detection = "speed40"
                    elif "50" in object_name or "speed limit 50" in object_name.lower():
                        current_detection = "speed50"
                    elif "60" in object_name or "speed limit 60" in object_name.lower():
                        current_detection = "speed60"
                    elif "70" in object_name or "speed limit 70" in object_name.lower():
                        current_detection = "speed70"
                    elif "80" in object_name or "speed limit 80" in object_name.lower():
                        current_detection = "speed80"
                    elif "90" in object_name or "speed limit 90" in object_name.lower():
                        current_detection = "speed90"
                    elif "100" in object_name or "speed limit 100" in object_name.lower():
                        current_detection = "speed100"
                    elif "120" in object_name or "speed limit 120" in object_name.lower():
                        current_detection = "speed120"
                    elif "stop" in object_name.lower():
                        current_detection = "stop"
                    elif "red" in object_name.lower() and "light" in object_name.lower():
                        current_detection = "red_light"
                    elif "green" in object_name.lower() and "light" in object_name.lower():
                        current_detection = "green_light"
                    elif "yellow" in object_name.lower() and "light" in object_name.lower():
                        current_detection = "yellow_light"
                    
            # Check if detection has changed
            if current_detection != prev_detection:
                if current_detection == "speed30":
                    send_can_message(speed=SPEED_30)
                elif current_detection == "speed40":
                    send_can_message(speed=SPEED_40)
                elif current_detection == "speed50":
                    send_can_message(speed=SPEED_50)
                elif current_detection == "speed60":
                    send_can_message(speed=SPEED_60)
                elif current_detection == "speed70":
                    send_can_message(speed=SPEED_70)
                elif current_detection == "speed80":
                    send_can_message(speed=SPEED_80)
                elif current_detection == "speed90":
                    send_can_message(speed=SPEED_90)
                elif current_detection == "speed100":
                    send_can_message(speed=SPEED_100)
                elif current_detection == "speed120":
                    send_can_message(speed=SPEED_120)
                elif current_detection == "stop":
                    send_can_message(direction=STOP_SIGN)
                elif current_detection == "red_light":
                    send_can_message(direction=RED_LIGHT)
                elif current_detection == "green_light":
                    send_can_message(direction=GREEN_LIGHT)
                elif current_detection == "yellow_light":
                    send_can_message(direction=YELLOW_LIGHT)
                
                prev_detection = current_detection

            # Calculate and print FPS
            t2 = cv2.getTickCount()
            time1 = (t2 - t1) / freq
            frame_rate_calc = 1 / time1
            print(f"FPS: {frame_rate_calc:.2f}")

        except zmq.Again:
            # No new frame available
            continue

except KeyboardInterrupt:
    print("Interrupted")

# Clean up
cv2.destroyAllWindows()
socket.close()
context.term()
