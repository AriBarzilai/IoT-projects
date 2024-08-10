#include <Arduino.h>

#include "electraRemote\electraRemote.h"
#include "env.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// DHT PINS
#define DHT_PIN 15
#define DHT_TYPE DHT22

#define BLYNK_PRINT Serial // prints to serial

bool DEBUG_MODE = true;

char ssid[] = WIFI_SSID; // your network SSID (name)
char pass[] = WIFI_PASS;

ElectraRemote remote;
BlynkTimer timer;
DHT dht(DHT_PIN, DHT_TYPE);

void getRoomTemp();

void setup()
{
    Serial.begin(9600);
    remote = ElectraRemote();
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    dht.begin();
    timer.setInterval(3000L, getRoomTemp); // once every minute
}

double interval = 1000; // time between each check of light sensor
unsigned long currTime = -interval;

void loop()
{
    Blynk.run();
    timer.run();
}
int num = 10;

float humidity;
float temperature;
double aT; // apparent temperature
void getRoomTemp()
{
    double tempH = dht.readHumidity();
    double tempT = dht.readTemperature();
    if (!isnan(tempH) && !isnan(tempT))
    {
        humidity = tempH;
        temperature = tempT;
        aT = 1.1 * temperature + 0.0261 * humidity - 3.944; // apparent (perceived) temperature. simple formula taken from online;
        Blynk.virtualWrite(V5, aT);
        Serial.print("Room temp: ");
        Serial.println(aT);
    }
    else
    {
        timer.restartTimer(0);
    }
}

// POWER STATE (on/off)
BLYNK_WRITE(V0)
{
    int pinValue = param.asInt();

    PowerState pwr = (PowerState)pinValue;
    remote.setPowerState(pwr);
    if (DEBUG_MODE)
    {
        Serial.print("Power: ");
        Serial.println(pwr == PowerState::ON ? "ON" : "OFF");
    }
}

// TEMPERATURE MODE (cold/hot)
BLYNK_WRITE(V1)
{
    int pinValue = param.asInt();

    TemperatureMode tempMode = (TemperatureMode)pinValue;
    remote.setTemperatureMode(tempMode);
    if (DEBUG_MODE)
    {
        Serial.print("Temperature Mode: ");
        Serial.println(tempMode == TemperatureMode::COLD ? "COLD" : "HOT");
    }
}

// FAN SPEED (1/2/3/auto)
BLYNK_WRITE(V2)
{
    int pinValue = param.asInt();

    FanSpeed fan = (FanSpeed)pinValue;
    remote.setFanSpeed(fan);
    if (DEBUG_MODE)
    {
        Serial.print("Fan Speed: ");
        switch (fan)
        {
        case FanSpeed::FAN_1:
            Serial.println("1");
            break;
        case FanSpeed::FAN_2:
            Serial.println("2");
            break;
        case FanSpeed::FAN_3:
            Serial.println("3");
            break;
        case FanSpeed::FAN_AUTO:
            Serial.println("AUTO");
            break;
        }
    }
}

// TEMPERATURE (16-30)
BLYNK_WRITE(V3)
{
    int pinValue = param.asInt();

    remote.setTemperature(pinValue);
    if (DEBUG_MODE)
    {
        Serial.print("Temperature: ");
        Serial.println(pinValue);
    }
}
