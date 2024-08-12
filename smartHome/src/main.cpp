#include <ESP32Servo.h>
#include <Arduino.h>

#include "env.h"
#include "electraRemote\electraRemote.h"
#include "presenceDetection\tenantHandler.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// Temperature sensor
#define DHT_PIN 15
#define DHT_TYPE DHT22

enum State
{
    DETECT_CLIMATE,
    DETECT_PRESENCE,
    CONTROL_CLIMATE,
    CONTROLE_PLANTS
};
State currState = DETECT_CLIMATE;

bool DEBUG_MODE = true;

char ssid[] = WIFI_SSID; // your network SSID (name)
char pass[] = WIFI_PASS;

ElectraRemote remote;
BlynkTimer timer;
DHT dht(DHT_PIN, DHT_TYPE);
TenantHandler tenantHandler;

PowerState pwr;
TemperatureMode tempMode;
FanSpeed fanSpeed;
int acTemperature;

int currentTimerID;

void call_controlClimate();
void call_plantControl();
void getRoomTemp();
void call_ping();
void acSetPwr(int pinValue);
void acSetTempMode(int pinValue);
void acSetTemperature(int pinValue);

void setup()
{
    Serial.begin(9600);

    remote = ElectraRemote();
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    dht.begin();
    tenantHandler.initTenants();

    currentTimerID = timer.setInterval(3000L, getRoomTemp);
    Serial.println("BEGIN SMART HOME SYSTEM");
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
        Serial.println("CLIMATE DETECTION");
        humidity = tempH;
        temperature = tempT;
        aT = 1.1 * temperature + 0.0261 * humidity - 3.944; // apparent (perceived) temperature. simple formula taken from online;
        Blynk.virtualWrite(V5, aT);
        Serial.print("Room temp: ");
        Serial.println(aT);

        timer.deleteTimer(currentTimerID);
        Serial.println("PRESENCE DETECTION");
        currentTimerID = timer.setInterval(PING_BETWEEN_TENANTS, call_ping);
    }
}

long pingCooldown = PING_BEFORE_TENANTS;
long lastTenantPing = -pingCooldown;
void call_ping()
{
    if (millis() - lastTenantPing < pingCooldown)
    {
        timer.deleteTimer(currentTimerID);
        currentTimerID = timer.setInterval(5000L, call_controlClimate);
        return;
    }

    tenantHandler.pingNextTenant();
    if (tenantHandler.allTenantsPinged())
    {
        pingCooldown = PING_BEFORE_TENANTS;
        lastTenantPing = millis();

        timer.deleteTimer(currentTimerID);
        currentTimerID = timer.setInterval(5000L, call_controlClimate);
    }
}

long climateControlCooldown = 1200000L;
long lastClimateOverride = -climateControlCooldown;
void call_controlClimate()
{
    Serial.println("AUTO CLIMATE CONTROL");
    if (millis() - lastClimateOverride < climateControlCooldown)
    {
        Serial.println("Skip");
        timer.deleteTimer(currentTimerID);
        currentTimerID = timer.setInterval(3000L, call_plantControl);
        return;
    }

    if (tenantHandler.getHomeTenantsCount() == 0)
    {
        if (pwr == PowerState::ON) // turn off AC
        {
            Blynk.virtualWrite(V0, (int)PowerState::OFF);
            acSetPwr((int)PowerState::OFF);
        }
    }
    else
    {
        if (pwr == PowerState::ON)
        {
            if (tempMode == TemperatureMode::COLD)
            {
                if (aT <= 10) // switch to heat mode
                {
                    Blynk.virtualWrite(V1, (int)TemperatureMode::HOT);
                    acSetTempMode((int)TemperatureMode::HOT);
                }
                else if (aT <= 18) // turn off the AC
                {
                    Blynk.virtualWrite(V0, (int)PowerState::OFF);
                    acSetPwr((int)PowerState::OFF);
                }
                else
                {
                    Serial.println("AC cooling still necessary");
                }
            }
            else // TEMPMODE HOT
            {
                if (aT >= 27) // switch to cold mode
                {
                    Blynk.virtualWrite(V1, (int)TemperatureMode::COLD);
                    acSetTempMode((int)TemperatureMode::COLD);
                }
                else if (aT >= 22) // turn off the AC
                {
                    Blynk.virtualWrite(V0, (int)PowerState::OFF);
                    acSetPwr((int)PowerState::OFF);
                }
                else
                {
                    Serial.println("AC heating still necessary");
                }
            }
        }
        else
        {                 // POWER OFF
            if (aT <= 15) // turn on the AC on heat mode
            {
                Blynk.virtualWrite(V0, (int)PowerState::ON);
                Blynk.virtualWrite(V1, (int)TemperatureMode::HOT);
                Blynk.virtualWrite(V3, 25);
                acSetPwr((int)PowerState::ON);
                acSetTempMode((int)TemperatureMode::HOT);
                acSetTemperature(24);
            }
            else if (aT >= 25) // turn on the AC on cold mode
            {
                Blynk.virtualWrite(V0, (int)PowerState::ON);
                Blynk.virtualWrite(V1, (int)TemperatureMode::COLD);
                Blynk.virtualWrite(V3, 16);
                acSetPwr((int)PowerState::ON);
                acSetTempMode((int)TemperatureMode::COLD);
                acSetTemperature(16);
            }
        }
    }
    timer.deleteTimer(currentTimerID);
    currentTimerID = timer.setInterval(5000L, call_plantControl);
}

void call_plantControl()
{
    // control the plants
    Serial.println("PLANT CONTROL");
    timer.deleteTimer(currentTimerID);
    currentTimerID = timer.setInterval(3000L, getRoomTemp);
}

void acSetPwr(int pinValue)
{
    pwr = (PowerState)pinValue;
    remote.setPowerState(pwr);
    if (DEBUG_MODE)
    {
        Serial.print("Power: ");
        Serial.println(pwr == PowerState::ON ? "ON" : "OFF");
    }
}

void acSetTempMode(int pinValue)
{
    tempMode = (TemperatureMode)pinValue;
    remote.setTemperatureMode(tempMode);
    if (DEBUG_MODE)
    {
        Serial.print("Temperature Mode: ");
        Serial.println(tempMode == TemperatureMode::COLD ? "COLD" : "HOT");
    }
}

void acSetTemperature(int pinValue)
{
    acTemperature = pinValue;
    remote.setTemperature(acTemperature);
    if (DEBUG_MODE)
    {
        Serial.print("Temperature: ");
        Serial.println(acTemperature);
    }
}

// POWER STATE (on/off)
BLYNK_WRITE(V0)
{
    Serial.print("MANUAL: ");
    acSetPwr(param.asInt());
}

// TEMPERATURE MODE (cold/hot)
BLYNK_WRITE(V1)
{
    Serial.print("MANUAL: ");
    acSetTempMode(param.asInt());
}

// // FAN SPEED (1/2/3/auto)
// BLYNK_WRITE(V2)
// {
//     int pinValue = param.asInt();

    fanSpeed = (FanSpeed)pinValue;
    remote.setFanSpeed(fanSpeed);
    if (DEBUG_MODE)
    {
        Serial.print("Fan Speed: ");
        switch (fanSpeed)
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
    Serial.print("MANUAL: ");
    int pinValue = param.asInt();
}
