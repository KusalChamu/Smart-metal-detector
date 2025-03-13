# Smart-metal-detector
Introduction

Traditional metal detectors lack intelligent capabilities and require manual operation. This project aims to enhance detection accuracy and usability by integrating IoT and advanced sensors.

Team Members

Ariyarathna K.T.S.P.

Gamage M.G.C.T.

Wijesooriya G.G.P.N.

Amarasooriya K.A.C.T.

Silva K.C.D.

Problems statement

Metal detection is a critical task in various fields, such as security, archaeology, and landmine detection. However, manually scanning large areas is time-consuming and inefficient. A robotic system that autonomously scans an area and reports metal detections can significantly improve accuracy and efficiency

Objectives

Automate metal detection in a specific area using a zigzag search pattern.

Continuously scan for metal while moving and report detected locations.

Integrate GPS to log the coordinates of detected metal objects.

Send real-time alerts to a mobile application via Bluetooth communication.

Ensure ease of operation with an ESP32-controlled system that requires minimal human intervention

System Overview

Components:

Metal Detection: Inductive/Capacitive sensors.

Processing Unit: ESP32

Connectivity: Bluetooth/Wi-Fi.

UI: Mobile app (Blynk/MIT App Inventor).

Locomotion:4x Gear Motors

Driving Motors:L298 Motor Driver

Power: Rechargeable Li-ion battery.


System workflow:

The robot moves to cover a 10m x 10m area.Then metal detector circuit continuously scans for metal.If metal is found, GPS location is read using NEO-6M GPS module. ESP32 sends alert message (GPS coordinates + "Metal Found") to your mobile app. Motors are controlled by ESP32 directly.

