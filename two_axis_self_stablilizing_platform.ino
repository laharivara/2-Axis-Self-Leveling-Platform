#include <Wire.h>
#include <Servo.h>

#define MPU_ADDR 0x68

Servo servoFB;   // D10 - Forward/Backward
Servo servoLR;   // D9  - Left/Right

// SERVO CENTER

const int CENTER_FB = 90;
const int CENTER_LR = 90;

// CONTROL GAIN

float Kp_FB = 1.5;
float Kp_LR = 1.5;

// SERVO LIMITS

const int MIN_ANGLE = 0;
const int MAX_ANGLE = 180;

// CALIBRATION OFFSETS

float rollOffset = 0;
float pitchOffset = 0;

// FILTERED ANGLES

float filteredRoll = 0;
float filteredPitch = 0;

// Lower = smoother, higher = faster response
const float FILTER = 0.10;

// READ MPU6500

void readMPU(int16_t &ax, int16_t &ay, int16_t &az)
{
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B);
    Wire.endTransmission(false);

    Wire.requestFrom(MPU_ADDR, 6);

    ax = (Wire.read() << 8) | Wire.read();
    ay = (Wire.read() << 8) | Wire.read();
    az = (Wire.read() << 8) | Wire.read();
}

// CALCULATE TILT

void getTilt(float &roll, float &pitch)
{
    int16_t axRaw, ayRaw, azRaw;

    readMPU(axRaw, ayRaw, azRaw);

    float ax = (float)axRaw;
    float ay = (float)ayRaw;
    float az = (float)azRaw;

    roll = atan2(ay, az) * 180.0 / PI;

    pitch = atan2(-ax,sqrt(ay * ay + az * az)) * 180.0 / PI;

    roll -= rollOffset;
    pitch -= pitchOffset;
}

// CALIBRATION

void calibrate()
{
    Serial.println("KEEP PLATFORM LEVEL");
    Serial.println("Calibrating...");

    delay(2000);

    float rollSum = 0;
    float pitchSum = 0;

    const int samples = 500;

    for (int i = 0; i < samples; i++)
    {
        int16_t axRaw, ayRaw, azRaw;

        readMPU(axRaw, ayRaw, azRaw);

        float ax = (float)axRaw;
        float ay = (float)ayRaw;
        float az = (float)azRaw;

        float roll =atan2(ay, az) * 180.0 / PI;

        float pitch =atan2(-ax,sqrt(ay * ay + az * az)) * 180.0 / PI;

        rollSum += roll;
        pitchSum += pitch;

        delay(5);
    }

    rollOffset = rollSum / samples;
    pitchOffset = pitchSum / samples;

    Serial.println("Calibration complete.");
    Serial.print("Roll offset: ");
    Serial.println(rollOffset);
    Serial.print("Pitch offset: ");
    Serial.println(pitchOffset);
}

void setup()
{
    Serial.begin(115200);

    Wire.begin();

    // Wake MPU6500
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x6B);
    Wire.write(0x00);
    Wire.endTransmission();

    delay(100);

    servoFB.attach(10);
    servoLR.attach(9);

    servoFB.write(90);
    servoLR.write(90);

    delay(1500);

    calibrate();

    // Start filter at zero
    filteredRoll = 0;
    filteredPitch = 0;

    delay(500);
}

void loop()
{
    float roll, pitch;

    getTilt(roll, pitch);

    // SMOOTH THE SENSOR READINGS

    filteredRoll =FILTER * roll +(1.0 - FILTER) * filteredRoll;

    filteredPitch =FILTER * pitch +(1.0 - FILTER) * filteredPitch;

    // SELF LEVELING

    int fbAngle =CENTER_FB + (filteredRoll * Kp_FB);

    int lrAngle =CENTER_LR + (filteredPitch * Kp_LR);

    // LIMIT SERVO MOVEMENT

    fbAngle = constrain(fbAngle,MIN_ANGLE,MAX_ANGLE);

    lrAngle = constrain(lrAngle,MIN_ANGLE,MAX_ANGLE);

    // MOVE SERVOS

    servoFB.write(fbAngle);
    servoLR.write(lrAngle);

    // SERIAL MONITOR

    Serial.print("Roll: ");
    Serial.print(filteredRoll, 2);
    Serial.print("  Pitch: ");
    Serial.print(filteredPitch, 2);
    Serial.print("  FB: ");
    Serial.print(fbAngle);
    Serial.print("  LR: ");
    Serial.println(lrAngle);

    delay(20);
}