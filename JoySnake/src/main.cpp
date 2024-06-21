#include <Arduino.h>
#include <BleKeyboard.h>

BleKeyboard bleJoystick("joystick");
char key, prevKey;

bool play = false;
bool quit = false;
bool isPaused = true;

void mpuJoystickMode();
void pauseMode();
void quitMode();
void initializeMPU();
void initializeEncoder();
void initializeGodMode();
bool getStatus();
char mpuToJoystick();
char getGodModeKey();
bool godModeIsActive();

void setup()
{

  Serial.begin(9600);

  bleJoystick.begin();
  while (!bleJoystick.isConnected())
    ;
  initializeMPU();

  // initializeEncoder();
  // initializeGodMode();
}

void loop()
{
  // isPaused = getStatus();
  // if (play)
  //   mpuJoystickMode();
  // else if (isPaused)
  //   pauseMode();
  // else if (quit)
  //   quitMode();
  key = mpuToJoystick();
  if (key != ' ')
  {
    bleJoystick.write(key);
    prevKey = key;
  }
}

void mpuJoystickMode()
{

  if (godModeIsActive())
  {
    key = getGodModeKey();
    bleJoystick.write(key);
  }
  else if (key != ' ')
  {
    key = mpuToJoystick();
    bleJoystick.write(key);

    prevKey = key;
  }
}

void pauseMode()
{
}

void quitMode()
{
}
