# 🚗 Advanced Driver Assistance System (ADAS) – Graduation Project 2025

An integrated, multi-module ADAS platform developed as part of our graduation project at Alexandria University. The system combines embedded systems, AI, computer vision, and real-time control to implement key safety features found in modern autonomous vehicles.

---

## 📌 Project Overview

This project demonstrates a fully functional **Advanced Driver Assistance System (ADAS)** prototype, featuring:

- **Blind Spot Detection (BSD)**
- **Adaptive Cruise Control (ACC)**
- **Traffic Sign Recognition (TSR)**
- **Automatic Parking (Parallel & Perpendicular)**
- **Autonomous Lane Change & Lane Keeping**
- **Driver Drowsiness Detection (DDD)**

All modules are deployed on a two-tier embedded platform: **STM32F401** for real-time control tasks and **Raspberry Pi 5** for AI-driven perception systems.

---

## 🎯 Objectives

- Increase road safety by automating driver support tasks.
- Design a real-time embedded control system with multiple sensors and communication protocols.
- Integrate AI and computer vision models for driver monitoring and scene understanding.
- Create a mobile, testable platform inspired by the **Tesla Cybertruck** design.

---

## ⚙️ System Architecture

### Hardware Components
- **STM32F401RCT6** (Bare-metal + FreeRTOS)
- **Raspberry Pi 5** (Python + Linux)
- **ESP32**, **MPU6050**, **HMC5883L**, **Ultrasonic Sensors**
- **CAN Bus Modules (MCP2515)**
- **Camera Module** (720p)
- **4WD Mecanum Wheel Platform**
- **Li-Ion Battery Packs + Buck/Boost Converters**

### Software Stack
| Layer | Tools & Libraries |
|------|-------------------|
| MCU | STM32CubeIDE, HAL, FreeRTOS |
| SBC | Python, OpenCV, TensorFlow Lite, YOLOv5 |
| Communication | CAN, SPI, I2C, UART |
| GUI | Custom Tkinter for monitoring/control |
| AI | Custom-trained models with Roboflow & TensorFlow |

---

## 🧠 Features In-Depth

### ✅ Adaptive Cruise Control (ACC)
- Ultrasonic-based distance sensing
- PID-based speed control on STM32
- Safe distance maintenance

### ✅ Blind Spot Detection (BSD)
- Lateral sonar monitoring
- CAN-based alert messaging
- Buzzer and LED feedback

### ✅ Driver Drowsiness Detection (DDD)
- Facial landmarks detection via TensorFlow Lite
- Eye closure, yawning, and head orientation tracking
- Real-time inference on Raspberry Pi

### ✅ Traffic Sign Recognition (TSR)
- Custom YOLOv5 model trained via Roboflow
- Real-time camera feed processing
- CAN-integrated command generation

### ✅ Auto Parking
- Environment mapping via ultrasonic sensors
- Control logic for both parallel and perpendicular parking
- Mecanum wheel platform control via PWM

### ✅ Lane Keeping & Auto Lane Change
- HSV-based lane detection in OpenCV
- Region of Interest (ROI) segmentation
- Dynamic steering control with safety checks
