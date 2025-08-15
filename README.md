Arduino Fall Detection System with GPS & GSM
This project is an open-source, Arduino-based system designed to detect falls and automatically send emergency alerts. It integrates motion sensing, GPS location tracking, and GSM communication into a single, portable device. The system is ideal for personal safety applications, such as for the elderly or for lone workers.

Features:

Accurate Fall Detection: Uses a three-stage algorithm to identify a fall based on acceleration and orientation changes.
Real-time Location Tracking: Acquires latitude and longitude data from a GPS module.
Automated Emergency Alerts: Sends an SMS with the exact location to multiple pre-defined contacts.
Recovery Notification: Automatically sends a separate "recovered" SMS if the user stands up after a fall is detected.


Hardware Requirements:

Arduino Board (e.g., Arduino Uno)
MPU6050 Accelerometer and Gyroscope Sensor
GPS Module (e.g., GY-NEO6MV2)
GSM Module (e.g., SIM800L)
Jumper Wires and a Breadboard

Software & Library Dependencies:

This project requires the following libraries, which can be installed via the Arduino IDE's Library Manager:
I2Cdev 
MPU6050 
TinyGPSPlus 
SoftwareSerial 

Setup Instructions:

1. Wiring
MPU6050: Connect the MPU6050 to the Arduino using I2C communication (SDA to A4, SCL to A5).
GPS Module: Connect the GPS RX pin to Arduino digital pin 4 and the GPS TX pin to digital pin 3.
GSM Module: Connect the GSM RX pin to Arduino digital pin 10 and the GSM TX pin to digital pin 11.

2. Configuration
Open the fall_detection_code.ino file in the Arduino IDE.
Phone Numbers: In the code, modify the number array with the emergency contact numbers you wish to send alerts to.

Calibration: The AcX, AcY, AcZ and GyX, GyY, GyZ values for normalization may need to be calibrated for your specific sensor to ensure accuracy.

How It Works
The system continuously monitors motion data from the MPU6050. A fall is detected based on a three-trigger sequence:
Trigger 1: The Amplitude Vector (AM) of acceleration drops below a threshold, indicating a free-fall state.
Trigger 2: The AM exceeds a high threshold shortly after, indicating an impact.
Trigger 3: A significant change in orientation (calculated from gyroscope data angleChange) confirms the fall event.

Upon a successful detection, the system acquires the location from the GPS module. It then sends an SMS to the designated contacts using the 

SendMessage function. If the user recovers, the 
SendMessageR function is called to send a "Recovered from Fall" alert.
