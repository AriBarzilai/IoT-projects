#include <Arduino.h>

#define REFRIGERATOR_PIN 35
#define FREEZER_PIN 34

#define BUZZER_PIN 33

// #define INTERVAL 180000 // 3 minutes

#define INTERVAL 10000 // 10 seconds

bool isRefrigeratorOpen = false;
bool isFreezerOpen = false;

bool alarmTriggred = false;

unsigned long openTime = 0;

void fridgeOpen();

void setup()
{
  Serial.begin(9600);
  pinMode(REFRIGERATOR_PIN, INPUT);
  pinMode(FREEZER_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop()
{

  Serial.print("Refrigerator ");
  Serial.print((digitalRead(REFRIGERATOR_PIN) == 0) ? "Open" : "Closed");
  Serial.print("\t\t");
  Serial.print("Freezer ");
  Serial.println((digitalRead(FREEZER_PIN) == 0) ? "Open" : "Closed");
  // isRefrigeratorOpen = digitalRead(REFRIGERATOR_PIN) == 0;
  // isFreezerOpen = digitalRead(FREEZER_PIN) == 0;
  // if (isRefrigeratorOpen || isFreezerOpen)
  // {
  //   fridgeOpen();
  // } else {
  //   if (openTime >= INTERVAL)
  //   {
  //     Serial.println("Door closed");
  //     Serial.print("Door was open for ");
  //     Serial.print((millis() - openTime) / 1000);
  //     Serial.println(" seconds");
  //   }
  //   openTime = 0;
  // }
}

void playAlaram()
{
  Serial.println("Playing alarm");
}

void fridgeOpen()
{
  openTime = (openTime == 0) ? millis() : openTime;

  if ((millis() - openTime) > INTERVAL)
  {
    playAlaram();
  }
}
