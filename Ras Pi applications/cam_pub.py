import cv2
import zmq
import time

def video_publisher(port=5555):
    # Initialize ZeroMQ context and PUB socket with CONFLATE option
    context = zmq.Context()
    socket = context.socket(zmq.PUB)
    socket.setsockopt(zmq.CONFLATE, 1)  # Keep only last message
    socket.bind(f"tcp://127.0.0.1:5555")

    cap = cv2.VideoCapture(0)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)
    if not cap.isOpened():
        print("Error: Could not open video source")
        return

    try:
        while True:
            ret, frame = cap.read()
            if not ret:
                break

            # Compress frame to JPEG
            _, jpeg_frame = cv2.imencode('.jpg', frame, [cv2.IMWRITE_JPEG_QUALITY, 80])

            # Send only the most recent frame (drops older unsent frames)
            socket.send(jpeg_frame.tobytes(), zmq.NOBLOCK)

            # Control frame rate
            #time.sleep(0.033)  # ~30 FPS

    except KeyboardInterrupt:
        print("Publisher stopped by user")
    finally:
        cap.release()
        socket.close()
        context.term()

if __name__ == "__main__":
    video_publisher()
