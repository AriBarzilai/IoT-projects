#include <Arduino.h>
#include "Wire.h"
#include <BleKeyboard.h>

BleKeyboard bleJoystick("joystick");

bool play = false;
bool quit = false;
bool pause = true;

void setup()
{

  Wire.begin();
  Serial.begin(115200);

  bleJoystick.begin();
  while (!bleJoystick.isConnected())
    ;
}

void loop()
{
  pause = getStatus();
  
  if (play)
    playMode();
  else if (pause)
    pauseMode();
  else if (quit)
    quitMode();
}



void playMode()
{
}

void pauseMode()
{
}

void quitMode()
{
}
