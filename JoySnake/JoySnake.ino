#include "MPU6050_6Axis_MotionApps_V6_12.h"

#include "Wire.h"

#include <BleKeyboard.h>

// MPU control range of vars
#define MPU_RANGE_MIN 80
#define MPU_RANGE_MAX 170
#define MPU_RANGE_2_MIN 230
#define MPU_RANGE_2_MAX 320


#define ENCODER_BUTTON 27

BleKeyboard bleJoystick("joystick");

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
float maxYaw, maxRoll, yaw, roll;

char tmpKey;
char key;

bool JoystickOnOff = false;

volatile bool mpuInterrupt = false;
void IRAM_ATTR dmpDataReady()
{
  mpuInterrupt = true;
}

void setup()
{
  
  
  
  Wire.begin();
  Serial.begin(115200);

  
  
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
  
  bleJoystick.begin();
  while (!bleJoystick.isConnected())
  {
    Serial.print(".");
  }
  Serial.println("");

  pinMode(ENCODER_BUTTON, INPUT_PULLUP);
}


void loop()
{
  if (digitalRead(ENCODER_BUTTON) == LOW)
    JoystickOnOff = !JoystickOnOff;
  
  if (JoystickOnOff)
    mpuToJoystick();
}


void mpuToJoystick()
{
  if (!readMPU())
    return;

  key = getMPU_key();

  if (key != ' ')
    bleJoystick.write(key);
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
float rangeMPU(float num)
{
  if (num < MPU_RANGE_MIN || (num > MPU_RANGE_MAX && num < MPU_RANGE_2_MIN) || num > MPU_RANGE_2_MAX)
    return -1;
  else
    return num;
}

char getMPU_key()
{

  tmpKey = ' ';

  // set in of range of 0-90 or -81 if mpu is not in range
  maxYaw = (yaw > 230) ? yaw - 230 : yaw - 80;
  maxRoll = (roll > 230) ? roll - 230 : roll - 80;

  if (maxYaw >= 0 && maxYaw > maxRoll)
    tmpKey = (yaw < MPU_RANGE_MAX) ? 'a' : 'd';
  else if (maxRoll >= 0)
    tmpKey = (roll < MPU_RANGE_MAX) ? 'w' : 's';

  return tmpKey;
}
