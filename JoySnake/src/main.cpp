#include <Arduino.h>
#include <BleKeyboard.h>

BleKeyboard bleJoystick("joystick");
char key;

bool play = false;
bool quit = false;
bool isPaused = true;

void initializeMPU();
char mpuToJoystick();

void initComboHandler();
void updateSensors();
bool hasPressedCombo();
char getComboSymbol();
void printDevice();

void setup()
{

  Serial.begin(9600);

  bleJoystick.begin();
  while (!bleJoystick.isConnected())
    ;
  initializeMPU();
  initComboHandler();
}

void loop()
{
  updateSensors();
  if (hasPressedCombo())
  {
    key = getComboSymbol();
    bleJoystick.write(key);
  }
  else if ((key = mpuToJoystick()) != ' ')
  {
    bleJoystick.write(key);
  }
}
