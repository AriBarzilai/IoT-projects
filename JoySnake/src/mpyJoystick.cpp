
#include "MPU6050_6Axis_MotionApps612.h"

MPU6050 mpu;

// MPU control range of vars
#define MPU_RANGE_MIN 70
#define MPU_RANGE_MAX MPU_RANGE_MIN + 90
#define MPU_RANGE_2_MIN 240
#define MPU_RANGE_2_MAX MPU_RANGE_2_MIN + 90

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
float maxYaw, maxRoll, yaw, roll;

char key, tmpKey;

volatile bool mpuInterrupt = false;
void IRAM_ATTR dmpDataReady()
{
  mpuInterrupt = true;
}

void begin()
{
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
    key = NULL;
    if (!readMPU())
        return NULL;

    key = getMPU_key();

    // if (key != ' ' && rollFlag != yawFlag)
    // {
    //     
    //     Serial.println(key);
    //     Serial.println();
    // }

    return key;
}

boolean readMPU()
{
    if (!dmpReady || !mpu.dmpGetCurrentFIFOPacket(fifoBuffer))
        return false;

    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

    yaw = rangeMPU((ypr[0] * 180 / M_PI) + 180); // set the range of yaw and roll to 0 360
    roll = rangeMPU((ypr[2] * 180 / M_PI) + 180);

    return true;
}

// if the num is in the range of valid intevals return the number else return
float rangeMPU(float value)
{
    if (value < MPU_RANGE_MIN || (value > MPU_RANGE_MAX && value < MPU_RANGE_2_MIN) || value > MPU_RANGE_2_MAX)
        return -1;
    else
        return value;
}

char getMPU_key()
{

    tmpKey = ' ';

    // set in of range of 0-90 or -81 if mpu is not in range
    maxYaw = (yaw > 230) ? yaw - 230 : yaw - 80;
    maxRoll = (roll > 230) ? roll - 230 : roll - 80;

    if (maxYaw >= 0 && maxYaw > maxRoll )
    {
        tmpKey = (yaw < MPU_RANGE_MAX) ? 'a' : 'd';
    }
    else if (maxRoll >= 0 )
    {
        tmpKey = (roll < MPU_RANGE_MAX) ? 'w' : 's';
    }

    return tmpKey;
}
