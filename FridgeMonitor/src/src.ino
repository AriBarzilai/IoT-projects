#include "keys.h" // This file contains the WIFI_SSID, WIFI_PASS and BLYNK_AUTH_TOKEN must be included first
#include "pitches.h"

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

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

BlynkTimer timer;
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // LED

bool currRefrigeratorOpen = false; // current refrigerator state (true = open)
bool currFreezerOpen = false;      // current freezer state
bool prevRefrigeratorOpen = false;
bool prevFreezerOpen = false;

bool refrigeratorAlarm = false;
bool freezerAlarm = false;

unsigned long openTime = 0;

double LEDinterval = 50;
double freezerInterval = 10000;
double refrigeratorInterval = 10000;
double playBuzzerInterval = 300;
double haltBuzzerInterval = 1000;
volatile unsigned long prevLEDUpdate = millis();
volatile unsigned long prevFreezerUpdate = millis();
volatile unsigned long prevRefrigeratorUpdate = millis();
volatile unsigned long prevBuzzerUpdate = millis();

bool soundBuzzer = true;

#define WEBHOOK_INTERVAL 240000 // 4 minutes in milliseconds
unsigned long lastWebhookTime = 0;

void myTimerEvent()
{
  String refrigeratorStatus = currRefrigeratorOpen ? "Open" : "Closed";
  String freezerStatus = currFreezerOpen ? "Open" : "Closed";

  Blynk.virtualWrite(V1, freezerStatus);
  Blynk.virtualWrite(V2, refrigeratorStatus);

  if (millis() - lastWebhookTime >= WEBHOOK_INTERVAL)
  {
    // Send the data to the webhook
    Blynk.logEvent("fridge_status", refrigeratorStatus + "," + freezerStatus);
    lastWebhookTime = millis();
  }
}

void setup()
{
  Serial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L, myTimerEvent);

  pinMode(REFRIGERATOR_PIN, INPUT);
  pinMode(FREEZER_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  ledcSetup(TONE_PWM_CHANNEL, 5000, 8);  // 5 kHz frequency, 8-bit resolution
  ledcAttachPin(BUZZER_PIN, TONE_PWM_CHANNEL); // Attach the channel to the pin

  strip.begin();
  strip.show();
  strip.setBrightness(150);
}

void loop()
{
  currFreezerOpen = digitalRead(FREEZER_PIN) == 0;
  currRefrigeratorOpen = digitalRead(REFRIGERATOR_PIN) == 0;

  Blynk.run();
  timer.run();

  if (currRefrigeratorOpen && (currRefrigeratorOpen != prevRefrigeratorOpen))
  {
    prevRefrigeratorUpdate = millis();
  }

  // loopLED();
  loopFreezer();
  loopRefrigerator();
  loopBuzzer();

  log();
  prevRefrigeratorOpen = currRefrigeratorOpen;
}

void loopFreezer()
{
  if (currFreezerOpen && (currFreezerOpen != prevFreezerOpen))
  {
    prevFreezerUpdate = millis();
  }
  else if (!currFreezerOpen)
  {
    prevFreezerUpdate = millis();
  }
  freezerAlarm = (millis() - prevFreezerUpdate > freezerInterval);
  prevFreezerOpen = currFreezerOpen;
}

void loopRefrigerator()
{
  if (currRefrigeratorOpen && (currRefrigeratorOpen != prevRefrigeratorOpen))
  {
    prevRefrigeratorUpdate = millis();
  }
  else if (!currRefrigeratorOpen)
  {
    prevRefrigeratorUpdate = millis();
  }
  refrigeratorAlarm = (millis() - prevRefrigeratorUpdate > freezerInterval);
  prevRefrigeratorOpen = currRefrigeratorOpen;
}

void loopLED()
{
  if (millis() - prevLEDUpdate > LEDinterval)
  {
    int red = (currFreezerOpen ? 255 : 0);
    for (int i = 0; i < (strip.numPixels() / 2); i++)
    {
      strip.setPixelColor(i, strip.Color(255, 0, 0));
    }
    red = (currRefrigeratorOpen ? 255 : 0);
    for (int i = (strip.numPixels() / 2); i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, strip.Color(red, 0, 0));
    }
    strip.show();
    prevLEDUpdate = millis();
  }
}

void loopBuzzer()
{
  if (!freezerAlarm && !refrigeratorAlarm)
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
    printLogMessage("ALARM", freezerAlarm || refrigeratorAlarm);
    printLogMessage("B_U", (double)(ms - prevBuzzerUpdate));

    Serial.println();
  }
}
