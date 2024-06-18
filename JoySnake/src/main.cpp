#include <Arduino.h>
#include "Wire.h"
#include <BleKeyboard.h>

BleKeyboard bleJoystick("joystick");

bool play = false;
bool quit = false;
bool isPaused = true;


  void playMode();
  void pauseMode();
  void quitMode();
  void initializeMPU();
  void initializeEncoder();
  void initializeGodMode();
  bool getStatus();

void setup()
{

  Wire.begin();
  Serial.begin(115200);

  bleJoystick.begin();
  while (!bleJoystick.isConnected())
    ;
  initializeMPU();
  initializeEncoder();
  initializeGodMode();
}

void loop()
{
  isPaused = getStatus();
  if (play)
    playMode();
  else if (isPaused)
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
