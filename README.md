# 2-Axis-Self-Leveling-Platform
A two-axis self-leveling platform built using an Arduino Uno, MPU6500 IMU, and two servo motors.

How It Works : 
The MPU6500 measures the platform's acceleration along
three axes. The Arduino uses these measurements to calculate
roll and pitch.
The readings are calibrated and filtered before being passed
to a proportional feedback controller. The controller then
adjusts the two servo motors to compensate for the detected tilt.

Current Features
- Roll and pitch measurement
- MPU6500 I²C communication
- Automatic calibration
- Proportional feedback control
- Two-axis servo control

Future Improvements
- PD/PID control
- Improved power supply
- Better mechanical stability
- More precise leveling

Components
 Component - Quantity 
 Arduino Uno - 1 
 MPU60500 - 1 
 Servo Motor - 2 
![2-Axis-Self-Leveling-Platform](platform.jpg)
