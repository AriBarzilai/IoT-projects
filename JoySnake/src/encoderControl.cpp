#include <Arduino.h>

#define ENCODER_BUTTON 27
#define ENCODER_PIN_A 25
#define ENCODER_PIN_B 26

volatile int encoderPos = 0;
int currentEncoderPos = 0;
int lastEncoderPos = 0;



void initializeEncoder()
{
  pinMode(ENCODER_BUTTON, INPUT_PULLUP);
  pinMode(ENCODER_PIN_A, INPUT);
  pinMode(ENCODER_PIN_B, INPUT);
}

bool getStatus()
{
  return true;
}