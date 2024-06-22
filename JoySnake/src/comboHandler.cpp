// handles "input combos" and sends a signal if a specific combo was pressed

#include <Arduino.h>
#include <vector>

#define LIGHT_SENSOR_PIN 34
#define ENCODER_BUTTON 27
#define ENCODER_PIN_A 25
#define ENCODER_PIN_B 26
#define MIN_LIGHT_SENSOR_THRESHOLD 1000

enum deviceType
{
  LIGHT_SENSOR,
  BUTTON,
  ENCODER,
};

enum encoderMovement
{
  STATIONARY,
  LEFT,
  RIGHT,
};

struct key
{
  deviceType device;
  int threshold;
};

key createComboKey(deviceType device, int threshold);
template <typename... Keys>
void createCombo(Keys... keys);
void updateSensors();
void updateCombo(deviceType device, int value);
int getEncoderDirection();
bool isMatch(deviceType device, int value);
bool hasPressedCombo();
char getComboSymbol();

// LIGHT SENSOR VARS
double interval = 500; // time between each check of light sensor
unsigned long currTime = 0.0;

unsigned long prevLightTime = -interval;
unsigned long prevEncoderTime = -interval;
unsigned long prevButtonTime = -interval;

int darkVal = 0; // how much lack of light does the light sensor sense

bool currComboStatus = false;
char comboSymbol = 'g'; // this symbol is sent to the PC to indicate a combo has been succesfully sent
std::vector<key> combo;
key *currentKey = nullptr;

volatile bool buttonPressed = false;
volatile int lastDirection = STATIONARY; // -1 for left, 1 for right, 0 for no movement

// Interrupt handler for encoder rotation
void IRAM_ATTR handleEncoderInterrupt()
{
  static int lastStateA = 0;
  int stateA = digitalRead(ENCODER_PIN_A);
  int stateB = digitalRead(ENCODER_PIN_B);

  if (stateA != lastStateA)
  { // Rotation detected
    lastDirection = (stateA == stateB) ? RIGHT : LEFT;
    lastStateA = stateA;
  }
}

// Interrupt handler for encoder button press
void IRAM_ATTR handleButtonInterrupt()
{
  buttonPressed = true;
}

void initComboHandler()
{

  pinMode(ENCODER_BUTTON, INPUT_PULLUP);
  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), handleEncoderInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_BUTTON), handleButtonInterrupt, FALLING);
  createCombo(
      createComboKey(ENCODER, LEFT),
      createComboKey(BUTTON, 0),
      createComboKey(ENCODER, RIGHT));
}

key createComboKey(deviceType device, int threshold)
{
  key key;
  key.device = device;
  key.threshold = threshold;
  return key;
}

template <typename... Keys>
void createCombo(Keys... keys)
{
  combo.clear();                // Clear existing keys
  (combo.push_back(keys), ...); // Expand and add new keys
  currentKey = &combo[0];       // Point to the first element
}

void updateSensors()
{
  currTime = millis();
  if (currTime - prevLightTime >= interval)
  {
    darkVal = analogRead(LIGHT_SENSOR_PIN);
    if (darkVal > MIN_LIGHT_SENSOR_THRESHOLD)
    {
      updateCombo(LIGHT_SENSOR, darkVal);
      prevLightTime = currTime;
    }
  }
  if (currTime - prevEncoderTime >= interval)
  {
    int direction = lastDirection;
    if (direction != STATIONARY)
    {
      updateCombo(ENCODER, direction);
      lastDirection = STATIONARY;
      prevEncoderTime = currTime;
    }
  }
  if (currTime - prevButtonTime >= interval)
  {
    if (buttonPressed == true)
    {
      updateCombo(BUTTON, 0);
      buttonPressed = !buttonPressed;
      prevButtonTime = currTime;
    }
  }
}

void updateCombo(deviceType device, int value)
{
  if (currentKey == nullptr)
  {
    return;
  }
  if (device == LIGHT_SENSOR && value < MIN_LIGHT_SENSOR_THRESHOLD)
  {
    return;
  }

  if (isMatch(device, value))
  {
    Serial.print("d: ");
    Serial.print(device);
    Serial.print("\tv: ");
    Serial.println(value);
    if (currentKey == &combo.back())
    {
      currComboStatus = true; // Last element matched, combo is successful
      Serial.println("Combo matched");
    }
    else
    {
      ++currentKey; // Move to the next element
    }
  }
  else
  {
    currentKey = &combo[0]; // Reset to the first element if not matched
    Serial.print("Reset combo");
    Serial.print("\td: ");
    Serial.print(device);
    Serial.print("\tv: ");
    Serial.println(value);

    if (isMatch(device, value))
    {
      if (currentKey == &combo.back())
      {
        currComboStatus = true; // Last element matched, combo is successful
        Serial.println("Combo matched");
      }
      else
      {
        ++currentKey; // Move to the next element
      }
    }
  }
}

// Function to get and reset the encoder direction
int getEncoderDirection()
{
  int direction = lastDirection;
  lastDirection = STATIONARY; // Reset after reading
  return direction;
}

// checks if the provided input matches the next expected input in the combo
bool isMatch(deviceType device, int value)
{
  if (currentKey == nullptr)
  {
    return false;
  }
  if (currentKey->device != device)
  {
    return false;
  }
  if (value >= currentKey->threshold)
  {
    return true;
  }
  return false;
}

bool hasPressedCombo()
{
  if (currComboStatus)
  {
    currComboStatus = false;
    return true;
  }
  return false;
}

char getComboSymbol()
{
  return comboSymbol;
}
