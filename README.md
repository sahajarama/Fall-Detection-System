# Fall-Detection-System
A fall detection system using an MPU6050 sensor, GPS, and GSM. It detects falls via a three-stage algorithm: sensing a drop in acceleration, a spike, and an orientation change. Upon detection, it sends an emergency SMS with a Google Maps link to pre-configured numbers. A "recovered" message is also sent if the user regains normal orientation.
