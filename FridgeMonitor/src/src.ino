#include "keys.h" // This file contains the WIFI_SSID, WIFI_PASS and BLYNK_AUTH_TOKEN must be included first
#include "pitches.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_NeoPixel.h>

#define REFRIGERATOR_PIN 35
#define FREEZER_PIN 34
#define BUZZER_PIN 19
#define LED_PIN 33

#define LED_COUNT 12
#define DELAY_TIME 300     // the number of milliseconds between the start of one sound and the next
#define PLAY_TIME 250      // the number of milliseconds to play a sound
#define TONE_PWM_CHANNEL 0 // the channel the buzzer plays in, we just put 0 for this

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

bool DEBUG_MODE = true;

char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

BlynkTimer timer;
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // LED

bool currRefrigeratorOpen = false; // current refrigerator state (true = open)
bool currFreezerOpen = false;      // current freezer state

#define ALARM_DELAY 180000 // 3 minutes

unsigned long doorOpenTime = 0; // 10 seconds
bool alarmStatus = false;
bool soundBuzzer = true;
bool isNotificationSent = false;

double playBuzzerInterval = 300;
double haltBuzzerInterval = 1000;
volatile unsigned long prevLEDUpdate = millis();
volatile unsigned long prevFreezerUpdate = millis();
volatile unsigned long prevRefrigeratorUpdate = millis();
volatile unsigned long prevBuzzerUpdate = millis();

void myTimerEvent()
{

  Blynk.virtualWrite(V1, currFreezerOpen ? "Open" : "Closed");
  Blynk.virtualWrite(V2, currRefrigeratorOpen ? "Open" : "Closed");
}

void setup()
{
  Serial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L, myTimerEvent);

  pinMode(REFRIGERATOR_PIN, INPUT);
  pinMode(FREEZER_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  
  strip.begin();
  strip.show();
  strip.setBrightness(150);
}

void loop()
{
  
  currFreezerOpen = digitalRead(FREEZER_PIN) == 0;           // 0 means open
  currRefrigeratorOpen = digitalRead(REFRIGERATOR_PIN) == 0; // 0 means open

  Blynk.run();
  timer.run();

  loopLED((currFreezerOpen ? 255 : 0), 0, (strip.numPixels() / 2));
  loopLED((currRefrigeratorOpen ? 255 : 0), (strip.numPixels() / 2), strip.numPixels());

  upddateAlarmStatus();
  loopBuzzer();
}

void loopLED(int red, int srtat, int end)
{
  for (int i = srtat; i < end; i++)
  {
    strip.setPixelColor(i, strip.Color(red, 0, 0));
  }
  strip.show();
}

void upddateAlarmStatus()
{
  if (currFreezerOpen || currRefrigeratorOpen)
  {
    doorOpenTime = (doorOpenTime == 0) ? millis() : doorOpenTime;

    if (!isNotificationSent && millis() - doorOpenTime > ALARM_DELAY)
    {
      Blynk.logEvent("fridg_open");
      isNotificationSent = true;
      alarmStatus = true;
    }
    else if (millis() - doorOpenTime > ALARM_DELAY)
    {
      alarmStatus = true;

    }
  }
  else
  {
    doorOpenTime = 0;
    alarmStatus = false;
    isNotificationSent = false;
  }
}

void loopBuzzer()
{

  if (!alarmStatus)
  {
    if (soundBuzzer)
    {
      noTone(BUZZER_PIN);
    }
    soundBuzzer = true; // reset alarms
    prevBuzzerUpdate = millis();
    return;
  }

  if (millis() - prevBuzzerUpdate > (soundBuzzer ? playBuzzerInterval : haltBuzzerInterval))
  {
    if (soundBuzzer)
    {
      tone(BUZZER_PIN, NOTE_D6);
    }
    else
    {
      noTone(BUZZER_PIN);
    }

    prevBuzzerUpdate = millis();
    soundBuzzer = !soundBuzzer;
  }
}

void printLogMessage(const char *name, int var)
{
  Serial.print(name);
  Serial.print(" ");
  Serial.print(var);
  Serial.print("\t");
}

void printLogMessage(const char *name, const char *var)
{
  Serial.print(name);
  Serial.print(" ");
  Serial.print(var);
  Serial.print("\t");
}

void printLogMessage(const char *name, bool var)
{
  Serial.print(name);
  Serial.print(" ");
  Serial.print(var ? "T" : "F");
  Serial.print("\t");
}

void printLogMessage(const char *name, double var)
{
  Serial.print(name);
  Serial.print(" ");
  Serial.print(var);
  Serial.print("\t");
}

void log()
{
  double ms = millis();
  if (DEBUG_MODE)
  {
    printLogMessage("F_O", currFreezerOpen);
    printLogMessage("R_O", currRefrigeratorOpen);
    printLogMessage("LED_U", (double)(ms - prevLEDUpdate));
    printLogMessage("F_U", (double)(ms - prevFreezerUpdate));
    printLogMessage("R_U", (double)(ms - prevRefrigeratorUpdate));
    // printLogMessage("ALARM", freezerAlarm || refrigeratorAlarm);
    printLogMessage("B_U", (double)(ms - prevBuzzerUpdate));

    Serial.println();
  }
}
