#include "keys.h" // This file contains the WIFI_SSID, WIFI_PASS and BLYNK_AUTH_TOKEN must be included first

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define REFRIGERATOR_PIN 35
#define FREEZER_PIN 34

#define BUZZER_PIN 33
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

// #define INTERVAL 180000 // 3 minutes

#define INTERVAL 10000 // 10 seconds

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

BlynkTimer timer;


bool isRefrigeratorOpen = false;
bool isFreezerOpen = false;

bool alarmTriggred = false;

unsigned long openTime = 0;


void myTimerEvent()
{
 Blynk.virtualWrite(V1, isFreezerOpen ? "Open" : "Closed");
 Blynk.virtualWrite(V2, isRefrigeratorOpen ? "Open" : "Closed");
}


void setup()
{
  Serial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L, myTimerEvent);

  pinMode(REFRIGERATOR_PIN, INPUT);
  pinMode(FREEZER_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop()
{
  isRefrigeratorOpen = digitalRead(REFRIGERATOR_PIN) == 0;
  isFreezerOpen = digitalRead(FREEZER_PIN) == 0;

  Blynk.run();
  timer.run();
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

// void playAlaram()
// {
//   Serial.println("Playing alarm");
// }

// void fridgeOpen()
// {
//   openTime = (openTime == 0) ? millis() : openTime;

//   if ((millis() - openTime) > INTERVAL)
//   {
//     playAlaram();
//   }
// }
