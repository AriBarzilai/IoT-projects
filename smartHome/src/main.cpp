#include <Arduino.h>

#include "electraRemote\electraRemote.h"
#include "env.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define BLYNK_PRINT Serial // prints to serial

bool DEBUG_MODE = true;

char ssid[] = SSID; // your network SSID (name)
char pass[] = PASS;

ElectraRemote remote;

void setup()
{
    Serial.begin(9600);
    remote = ElectraRemote();
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

double interval = 1000; // time between each check of light sensor
unsigned long currTime = -interval;

void loop()
{
    Blynk.run();
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

/*
// used for manual command input, for debugging
void parseCommand(String command)
{
    if (command == "on")
    {
        remote.setPowerState(PowerState::ON);
    }
    else if (command == "off")
    {
        remote.setPowerState(PowerState::OFF);
    }
    else if (command == "fan1")
    {
        remote.setFanSpeed(FanSpeed::FAN_1);
    }
    else if (command == "fan2")
    {
        remote.setFanSpeed(FanSpeed::FAN_2);
    }
    else if (command == "fan3")
    {
        remote.setFanSpeed(FanSpeed::FAN_3);
    }
    else if (command == "fanAuto")
    {
        remote.setFanSpeed(FanSpeed::FAN_AUTO);
    }
    else
    {
        // Try to parse the command as an integer for temperature
        int temp = command.toInt();
        if (temp != 0 || command == "0")
        { // toInt() returns 0 for invalid input
            remote.setTemperature(temp);
        }
        else
        {
            Serial.println("Invalid command");
        }
    }
}
*/