import cv2
import numpy as np
import zmq
from can import MCP2515
import time

# Initialize CAN controller
can_controller = MCP2515(bus=0, device=0, speed_hz=10000000)
if not can_controller.initialize(speed='1000kBPS'):
    print("Failed to initialize CAN controller")
    exit(1)

# Define CAN message ID
CAN_ID = 0x105

# Lane types (updated to match lane1.py)
LEFT_ONLY = 1
RIGHT_ONLY = 2
LEFT_RIGHT = 3
LANE_ERROR = 4

# Current status
current_lane_status = 0
current_drift_status = 0

# Lane status strings
LANE_STATUS_STRINGS = {
    LEFT_ONLY: "LEFT LANE ONLY",
    RIGHT_ONLY: "RIGHT LANE ONLY",
    LEFT_RIGHT: "BOTH LANES",
    LANE_ERROR: "LANE ERROR"
}


# Drift status (updated to match lane1.py)
DRIFT_RIGHT = 6
DRIFT_LEFT = 5
CENTRALIZED = 7
LEFT_CENTER = 8  # drift_center_left in lane1.py
RIGHT_CENTER = 9  # drift_center_right in lane1.py

# Drift status strings
DRIFT_STATUS_STRINGS = {
    DRIFT_RIGHT: "DRIFTING RIGHT",
    DRIFT_LEFT: "DRIFTING LEFT",
    CENTRALIZED: "CENTRALIZED",
    LEFT_CENTER: "SLIGHTLY LEFT",
    RIGHT_CENTER: "SLIGHTLY RIGHT"
}

# HSV color ranges
lower_yellow = np.array([15, 150, 20])
upper_yellow = np.array([35, 255, 255])
lower_black = np.array([0, 0, 0])
upper_black = np.array([180, 255, 30])
lower_white = np.array([0, 0, 150])
upper_white = np.array([180, 70, 255])

# Thresholds
thresold_denisty_yellow = 1.5
thresold_denisty_white_left_right = 2
thresold_denisty_white_center = 10
thresold_density_white_center_left_right = 1
thresold_denisty_black = 0

# Coordinates for regions of interest
x11, y11 = 330, 485
x12, y12 = 520, 300
x13, y13 = 350, 300
x14, y14 = 0, 485

x21, y21 = 620, 720
x22, y22 = 620, 540
x23, y23 = 290, 540
x24, y24 = 145, 720

x31, y31 = 1170, 720
x32, y32 = 1030, 540
x33, y33 = 620, 540
x34, y34 = 620, 720

x41, y41 = 1280, 485
x42, y42 = 930, 300
x43, y43 = 760, 300
x44, y44 = 950, 485

x51, y51 = 1280, 720
x52, y52 = 1280, 300
x53, y53 = 0, 300
x54, y54 = 0, 720

def region_of_interest(frame, x1, y1, x2, y2, x3, y3, x4, y4):
    """Define and mask region of interest"""
    quad = np.array([[(x1,y1), (x2,y2), (x3,y3), (x4,y4)]])
    mask = np.zeros_like(frame)
    cv2.fillPoly(mask, quad, [255,255,255])
    masked_frame = cv2.bitwise_and(frame, mask)
    return masked_frame

def calculate_density(hsv, color, area):
    """Calculate color density in a region"""
    if color == 'y':
        mask = cv2.inRange(hsv, lower_yellow, upper_yellow)
    elif color == 'w':
        mask = cv2.inRange(hsv, lower_white, upper_white)
    elif color == 'b':
        mask = cv2.inRange(hsv, lower_black, upper_black)
    else:
        return 0

    pixels_number = cv2.countNonZero(mask)
    density = (pixels_number / area) * 100
    return density

def polygon_area(vertices):
    """Calculate area of a polygon"""
    n = len(vertices)
    area = 0.0
    for i in range(n):
        x_i, y_i = vertices[i]
        x_j, y_j = vertices[(i + 1) % n]
        area += (x_i * y_j) - (x_j * y_i)
    return abs(area) / 2.0

# Calculate areas for each region
vertices1 = [(x11,y11), (x12,y12), (x13,y13), (x14,y14)]
vertices2 = [(x21,y21), (x22,y22), (x23,y23), (x24,y24)]
vertices3 = [(x31,y31), (x32,y32), (x33,y33), (x34,y34)]
vertices4 = [(x41,y41), (x42,y42), (x43,y43), (x44,y44)]
vertices5 = [(x51,y51), (x52,y52), (x53,y53), (x54,y54)]

area1 = polygon_area(vertices1)
area2 = polygon_area(vertices2)
area3 = polygon_area(vertices3)
area4 = polygon_area(vertices4)
area5 = polygon_area(vertices5)

# Initialize ZMQ SUB socket to receive from PUB1.py
context = zmq.Context()
socket = context.socket(zmq.SUB)
#socket.setsockopt(zmq.CONFLATE, 1)  # Keep only last message
socket.connect("tcp://127.0.0.1:5555")
socket.setsockopt_string(zmq.SUBSCRIBE, '')

def send_can_message(lane_status, drift_status):
    """Send CAN message with lane and drift information"""
    data = [lane_status, drift_status]
    can_controller.send_message(CAN_ID, data)

# Initialize timing variables
last_can_send_time = time.time()
can_send_interval = 0.25  # 250ms = 4 times per second
prev_time = time.time()

try:
    while True:
        # Receive frame from publisher (same as SUB1.py)
        frame_data = socket.recv()

        # Convert bytes to numpy array and decode
        nparr = np.frombuffer(frame_data, np.uint8)
        frame = cv2.imdecode(nparr, cv2.IMREAD_COLOR)

        if frame is None:
            print("Failed to decode frame")
            continue

        # Calculate FPS for current frame
        curr_time = time.time()
        fps = 1 / (curr_time - prev_time)
        prev_time = curr_time
        #print(f"LANE -->FPS: {fps:.2f}")

        # Process frame for lane detection
        frame1 = region_of_interest(frame, x11,y11,x12,y12,x13,y13,x14,y14)
        frame2 = region_of_interest(frame, x21,y21,x22,y22,x23,y23,x24,y24)
        frame3 = region_of_interest(frame, x31,y31,x32,y32,x33,y33,x34,y34)
        frame4 = region_of_interest(frame, x41,y41,x42,y42,x43,y43,x44,y44)
        frame5 = region_of_interest(frame, x51,y51,x52,y52,x53,y53,x54,y54)

        hsv1 = cv2.cvtColor(frame1, cv2.COLOR_BGR2HSV)
        hsv2 = cv2.cvtColor(frame2, cv2.COLOR_BGR2HSV)
        hsv3 = cv2.cvtColor(frame3, cv2.COLOR_BGR2HSV)
        hsv4 = cv2.cvtColor(frame4, cv2.COLOR_BGR2HSV)
        hsv5 = cv2.cvtColor(frame5, cv2.COLOR_BGR2HSV)

        yellow_right_density = calculate_density(hsv4, 'y', area4)
        white_right_density = calculate_density(hsv4, 'w', area4)
        white_left_density = calculate_density(hsv1, 'w', area1)
        white_left_center_density = calculate_density(hsv2, 'w', area2)
        white_right_center_density = calculate_density(hsv3, 'w', area3)
        white_center_density = calculate_density(hsv5, 'w', area5)

        # Determine drift status (updated to match lane1.py)
        if (white_left_center_density > thresold_density_white_center_left_right and
            white_left_center_density < thresold_denisty_white_center):
            drift_status = LEFT_CENTER
        elif (white_right_center_density > thresold_density_white_center_left_right and
              white_right_center_density < thresold_denisty_white_center):
            drift_status = RIGHT_CENTER

        elif white_left_center_density > thresold_denisty_white_center:
            drift_status = DRIFT_LEFT

        elif white_right_center_density > thresold_denisty_white_center:
            drift_status = DRIFT_RIGHT

        else:
            drift_status = CENTRALIZED


        # Determine lane status (updated to match lane1.py)
        if yellow_right_density > thresold_denisty_yellow:
            lane_status = LEFT_ONLY

        elif (white_right_density > thresold_denisty_white_left_right and
              white_left_density > thresold_denisty_white_left_right):
            lane_status = LEFT_RIGHT

        elif white_right_density > thresold_denisty_white_left_right:
            lane_status = RIGHT_ONLY

        else:
            lane_status = LANE_ERROR


        # If drifting significantly, mark as lane error (updated to match lane1.py)
        if drift_status in [DRIFT_LEFT, DRIFT_RIGHT]:
            lane_status = LANE_ERROR


        # Check if it's time to send CAN message (every 250ms)
        current_time = time.time()
        if current_time - last_can_send_time >= can_send_interval:
            send_can_message(lane_status, drift_status)
            print(f"Lane Status: {LANE_STATUS_STRINGS.get(lane_status, 'UNKNOWN')}")
            print(f"Drift Status: {DRIFT_STATUS_STRINGS.get(drift_status, 'UNKNOWN')}")
            last_can_send_time = current_time
        elif current_lane_status != lane_status or current_drift_status != drift_status:
            send_can_message(lane_status, drift_status)
            print(f"Lane Status: {LANE_STATUS_STRINGS.get(lane_status, 'UNKNOWN')}")
            print(f"Drift Status: {DRIFT_STATUS_STRINGS.get(drift_status, 'UNKNOWN')}")
            current_drift_status = drift_status
            current_lane_status = lane_status
except KeyboardInterrupt:
    print("Interrupted")

# Clean up
cv2.destroyAllWindows()
can_controller.close()
socket.close()
context.term()
