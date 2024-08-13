#include <ESP32Servo.h>
#include <Arduino.h>

#include "env.h"
#include "electraRemote\electraRemote.h"
#include "presenceDetection\tenantHandler.h"
#include "plantWateringSystem\plantWateringSystem.h"

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

char ssid[] = WIFI_SSID; // your network SSID (name)
char pass[] = WIFI_PASS;

plantWateringSystem plant;
bool plantWatering = false;

ElectraRemote remote;
BlynkTimer timer;
DHT dht(DHT_PIN, DHT_TYPE);
TenantHandler tenantHandler;

PowerState pwr;
TemperatureMode tempMode;
FanSpeed fanSpeed;
int acTemperature;

int currentTimerID;

bool isHome = false;

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
    plant = plantWateringSystem();
    plant.initialize();
    remote = ElectraRemote();
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    dht.begin();
    tenantHandler.initTenants();

    currentTimerID = timer.setInterval(3000L, getRoomTemp);
    DEBUG_PRINTLN("BEGIN SMART HOME SYSTEM");
}

double interval = 1000; // time between each check of light sensor
unsigned long currTime = -interval;

void loop()
{
    plant.update();
    Blynk.run();
    timer.run();
}

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

        timer.deleteTimer(currentTimerID);

        currentTimerID = timer.setInterval(PING_BETWEEN_TENANTS, call_ping);

        DEBUG_PRINTLN("CLIMATE DETECTION");
        DEBUG_PRINT("Room temp: ");
        DEBUG_PRINTLN(aT);
        DEBUG_PRINTLN("PRESENCE DETECTION");
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
    DEBUG_PRINT("AUTO CLIMATE CONTROL");
    if (millis() - lastClimateOverride < climateControlCooldown)
    {
        DEBUG_PRINTLN(" (OVERRIDEN)");
        timer.deleteTimer(currentTimerID);
        currentTimerID = timer.setInterval(3000L, call_plantControl);
        return;
    }
    DEBUG_PRINTLN("");

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
                climateHandleCold();
            }
            else // TEMPMODE HOT
            {
                climateHandleHeat();
            }
        }
        else // POWER OFF
        {
            if (aT <= 15) // turn on the AC on heat mode
            {
                acPredefHeat();
            }
            else if (aT >= 25) // turn on the AC on cold mode
            {
                acPredefCold();
            }
        }
    }
    timer.deleteTimer(currentTimerID);
    currentTimerID = timer.setInterval(5000L, call_plantControl);
}

void call_plantControl()
{
    // control the plants
    DEBUG_PRINTLN("PLANT CONTROL");

    if (plant.needsWatering() || plantWatering)
    {
        DEBUG_PRINTLN("watering the plant");
        plantWatering = false;
        plant.startWatering();
    }
    if (!plant.needsWatering())
    {
        timer.deleteTimer(currentTimerID);
        currentTimerID = timer.setInterval(3000L, getRoomTemp);
    }
}

void acSetPwr(int pinValue)
{
    pwr = (PowerState)pinValue;
    remote.setPowerState(pwr);
    DEBUG_PRINT("Power: ");
    DEBUG_PRINTLN(pwr == PowerState::ON ? "ON" : "OFF");
}

void acSetTempMode(int pinValue)
{
    tempMode = (TemperatureMode)pinValue;
    remote.setTemperatureMode(tempMode);
    DEBUG_PRINT("Temperature Mode: ");
    DEBUG_PRINTLN(tempMode == TemperatureMode::COLD ? "COLD" : "HOT");
}

void acSetTemperature(int pinValue)
{
    acTemperature = pinValue;
    remote.setTemperature(acTemperature);
    DEBUG_PRINT("Temperature: ");
    DEBUG_PRINTLN(acTemperature);
}

// Sets the AC to predefined settings for hot mode
void acPredefHeat()
{
    Blynk.virtualWrite(V0, (int)PowerState::ON);
    Blynk.virtualWrite(V1, (int)TemperatureMode::HOT);
    Blynk.virtualWrite(V3, 25);
    acSetPwr((int)PowerState::ON);
    acSetTempMode((int)TemperatureMode::HOT);
    acSetTemperature(24);
}

void acPredefCold()
{
    Blynk.virtualWrite(V0, (int)PowerState::ON);
    Blynk.virtualWrite(V1, (int)TemperatureMode::COLD);
    Blynk.virtualWrite(V3, 16);
    acSetPwr((int)PowerState::ON);
    acSetTempMode((int)TemperatureMode::COLD);
    acSetTemperature(16);
}

// Handles the climate control when the AC is on and temperature mode is on cold
void climateHandleCold()
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
        DEBUG_PRINTLN("AC cooling still necessary");
    }
}

// Handles the climate control when the AC is on and temperature mode is on heat
void climateHandleHeat()
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
        DEBUG_PRINTLN("AC heating still necessary");
    }
}

// POWER STATE (on/off)
BLYNK_WRITE(V0)
{
    DEBUG_PRINT("MANUAL: ");
    lastClimateOverride = millis();
    acSetPwr(param.asInt());
}

// TEMPERATURE MODE (cold/hot)
BLYNK_WRITE(V1)
{
    DEBUG_PRINT("MANUAL: ");
    lastClimateOverride = millis();
    acSetTempMode(param.asInt());
}

// FAN SPEED (1/2/3/auto)
BLYNK_WRITE(V2)
{
    int pinValue = param.asInt();

    fanSpeed = (FanSpeed)pinValue;
    remote.setFanSpeed(fanSpeed);
    if (DEBUG)
    {
        DEBUG_PRINT("Fan Speed: ");
        switch (fanSpeed)
        {
        case FanSpeed::FAN_1:
            DEBUG_PRINTLN("1");
            break;
        case FanSpeed::FAN_2:
            DEBUG_PRINTLN("2");
            break;
        case FanSpeed::FAN_3:
            DEBUG_PRINTLN("3");
            break;
        case FanSpeed::FAN_AUTO:
            DEBUG_PRINTLN("AUTO");
            break;
        }
    }
}

// TEMPERATURE (16-30)
BLYNK_WRITE(V3)
{
    DEBUG_PRINT("MANUAL: ");
    lastClimateOverride = millis();
    acSetTemperature(param.asInt());
}

// MANUAL PLANT WATERING
BLYNK_WRITE(V6)
{
    DEBUG_PRINTLN("MANUAL: Plant watering started");
    plantWatering = true;
}