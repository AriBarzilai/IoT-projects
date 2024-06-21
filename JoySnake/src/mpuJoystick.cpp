#include "MPU6050_6Axis_MotionApps612.h"
#include "Wire.h"

char mpuToJoystick();
boolean readMPU();
float range(float value);
char getMPU_key();

#define INTERVAL 50 // interval in milliseconds to read the MPU6050 data only 20 times per second

unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

MPU6050 mpu;

// MPU control/status vars
bool dmpReady = false;
uint8_t mpuIntStatus;
uint8_t devStatus;
uint16_t packetSize;
uint8_t fifoBuffer[64];

// orientation/motion vars
Quaternion q;
VectorFloat gravity;
float ypr[3];
float pitch, roll;

char tmpKey;

volatile bool mpuInterrupt = false;
void IRAM_ATTR dmpDataReady()
{
    mpuInterrupt = true;
}

void initializeMPU()
{
    Wire.begin();

    mpu.initialize();
    devStatus = mpu.dmpInitialize();

    if (devStatus == 0)
    {
        mpu.setDMPEnabled(true);
        attachInterrupt(digitalPinToInterrupt(23), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();
        dmpReady = true;
        packetSize = mpu.dmpGetFIFOPacketSize();
    }
}

char mpuToJoystick()
{

    if ((currentMillis = millis()) - previousMillis >= INTERVAL && readMPU())
    {
        previousMillis = currentMillis;
        return getMPU_key();
    }

    return ' ';
}

boolean readMPU()
{
    if (!dmpReady)
        return false;
    if (!mpu.dmpGetCurrentFIFOPacket(fifoBuffer))
        return false;
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    pitch = ypr[1] * 180 / M_PI;
    roll = ypr[2] * 180 / M_PI;

    return true;
}

// if the num is in the range of valid intevals return the number else return
bool range(int a, float value, int b)
{
    return (value > a && value < b) ? true : false;
}

char getMPU_key()
{
    tmpKey = ' ';
    if (range(-45, roll, 45))
    {
        if (range(-150, pitch, -45))
            tmpKey = 'a';
        else if (range(45, pitch, 150))
            tmpKey = 'd';
    }
    else if (range(-45, pitch, 45))
    {
        if (range(-150, roll, -45))
            tmpKey = 's';
        else if (range(45, roll, 150))
            tmpKey = 'w';
    }
    return tmpKey;
}
