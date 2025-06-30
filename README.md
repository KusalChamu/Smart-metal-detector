# Smart Metal Detector 

## Introduction
Traditional metal detectors lack intelligent capabilities and require manual operation.  
This project enhances detection accuracy and usability by integrating **IoT**, **GPS**, and **autonomous movement**, making metal detection faster, smarter, and more efficient.



## Team Members
- **Ariyarathna K.T.S.P.**
- **Gamage M.G.C.T.**
- **Wijesooriya G.G.P.N.**
- **Amarasooriya K.A.C.T.**
- **Silva K.C.D.**



## Problem Statement
- Metal detection is critical in fields like **security**, **archaeology**, and **landmine detection**.
- Manual scanning of large areas is **time-consuming** and **inefficient**.
- An **autonomous robotic system** capable of scanning and reporting metal detections can greatly improve **accuracy**, **safety**, and **efficiency**.



## Objectives
- ✅ **Automate** metal detection in a predefined area using a **zigzag search pattern**.
- ✅ **Continuously scan** the ground while moving and detect metals.
- ✅ **Log** the **GPS coordinates** of detected metal objects.
- ✅ **Send real-time alerts** to a mobile app via **Bluetooth communication**.
- ✅ **Minimize human involvement** with an **ESP32-controlled autonomous system**.



## System Overview

### Components:
- **Metal Detection**: Inductive/Capacitive sensors
- **Processing Unit**: ESP32
- **Connectivity**: Wi-Fi / Bluetooth
- **Location Tracking**: NEO-6M GPS Module
- **User Interface**: Web App 
- **Locomotion**: 4 × Gear Motors
- **Motor Driving Circuit**: L298 Motor Driver
- **Power Supply**: Rechargeable Li-ion Battery
  
##System Architecture

![metalarchi](https://github.com/user-attachments/assets/7d2a55c4-d060-47f3-96d9-1c08af8d169f)

### System Workflow:
- The robot **autonomously covers a 10m × 10m area** using a **zigzag pattern**.
- The **metal detector circuit** continuously scans for metals during movement.
- When metal is detected:
  - The **GPS module (NEO-6M)** reads the current location.
  - The **ESP32**:
    - Sends an **alert ("Metal Found")** along with **GPS coordinates** to a **mobile application** via Bluetooth.
    - Uploads the **GPS data to Firebase** (for cloud storage and monitoring if Wi-Fi is connected).
- The **motors** are precisely controlled by the ESP32 to maintain smooth navigation and scanning.



## Project Highlights
- 🌐 **Real-time data monitoring** via **Firebase**.
- 📍 **Precise location logging** for detected metals.
- 📱 **User-friendly mobile interface** for alerts and tracking.
- 🤖 **Autonomous movement** to minimize manual operation.
- 🔋 **Portable** with rechargeable batteries for field operation.

## Prototype Design


