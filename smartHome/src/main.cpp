#include <ESP32Servo.h>
#include <Arduino.h>

#include "env.h"
#include "climateHandler\climateControl.h"
#include "presenceDetection\tenantHandler.h"
#include "plantWateringSystem\plantWateringSystem.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// Temperature sensor
#define DHT_PIN 15
#define DHT_TYPE DHT22

State currState = DETECT_CLIMATE;

char ssid[] = WIFI_SSID; // your network SSID (name)
char pass[] = WIFI_PASS;

plantWateringSystem plant;
bool manualWaterOverride = false;

BlynkTimer timer;
DHT dht(DHT_PIN, DHT_TYPE);
TenantHandler tenantHandler;
ClimateControl climateControl;

int currentTimerID = -1;

bool isHome = false;

void call_controlClimate();
void call_plantControl();
void getRoomTemp();
void call_ping();
void switchToState(State newState);

void setup()
{
    Serial.begin(9600);
    plant = plantWateringSystem();
    plant.initialize();
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    dht.begin();
    tenantHandler.initTenants();
    climateControl = ClimateControl();

    switchToState(State::DETECT_CLIMATE);
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

        DEBUG_PRINTLN("CLIMATE DETECTION");
        DEBUG_PRINT("Room temp: ");
        DEBUG_PRINTLN(aT);
        DEBUG_PRINTLN("PRESENCE DETECTION");

        switchToState(State::DETECT_PRESENCE);
    }
}

long pingCooldown = PING_BEFORE_TENANTS;
long lastTenantPing = -pingCooldown;
void call_ping()
{
    if (millis() - lastTenantPing >= pingCooldown)
    {
        tenantHandler.pingNextTenant();
        if (tenantHandler.allTenantsPinged())
        {
            pingCooldown = PING_BEFORE_TENANTS;
            lastTenantPing = millis();
        }
    }
    switchToState(State::CONTROL_CLIMATE);
}

long climateControlCooldown = 1200000L;
long lastClimateOverride = -climateControlCooldown;
void call_controlClimate()
{
    DEBUG_PRINT("AUTO CLIMATE CONTROL");
    if (millis() - lastClimateOverride < climateControlCooldown)
    {
        DEBUG_PRINTLN(" (OVERRIDEN)");
        switchToState(State::CONTROL_PLANTS);
        return;
    }
    DEBUG_PRINTLN("");

    climateControl.updateTemperature(aT);
    if (tenantHandler.getHomeTenantsCount() == 0)
    {
        climateControl.onHouseEmpty();
    }
    else
    {
        climateControl.handleClimate();
    }
    switchToState(State::CONTROL_PLANTS);
}

void call_plantControl()
{
    // control the plants
    DEBUG_PRINTLN("PLANT CONTROL");

    if (plant.needsWatering() || manualWaterOverride)
    {
        DEBUG_PRINTLN("Watering plant");
        manualWaterOverride = false;
        plant.startWatering();
    }
    if (!plant.isWatering())
    {
        switchToState(State::DETECT_CLIMATE);
    }
}

void switchToState(State newState)
{
    currState = newState;
    timer.deleteTimer(currentTimerID);

    switch (currState)
    {
    case DETECT_CLIMATE:
        currentTimerID = timer.setInterval(DC_INTERVAL, getRoomTemp);
        break;
    case DETECT_PRESENCE:
        currentTimerID = timer.setInterval(DP_INTERVAL, call_ping);
        break;
    case CONTROL_CLIMATE:
        currentTimerID = timer.setInterval(CC_INTERVAL, call_controlClimate);
        break;
    case CONTROL_PLANTS:
        currentTimerID = timer.setInterval(CP_INTERVAL, call_plantControl);
        break;
    }
}

// POWER STATE (on/off)
BLYNK_WRITE(PWR_VPIN)
{
    DEBUG_PRINT("MANUAL: ");
    lastClimateOverride = millis();
    climateControl.airConditioner.setPowerState((PowerState)param.asInt());
}

// TEMPERATURE MODE (cold/hot)
BLYNK_WRITE(TMP_MD_VPIN)
{
    DEBUG_PRINT("MANUAL: ");
    lastClimateOverride = millis();
    climateControl.airConditioner.setTemperatureMode((TemperatureMode)param.asInt());
}

// FAN SPEED (1/2/3/auto)
BLYNK_WRITE(F_SPD_VPIN)
{
    DEBUG_PRINT("MANUAL: ");
    lastClimateOverride = millis();
    climateControl.airConditioner.setFanSpeed((FanSpeed)param.asInt());
}

// TEMPERATURE (16-30)
BLYNK_WRITE(TMP_VPIN)
{
    DEBUG_PRINT("MANUAL: ");
    lastClimateOverride = millis();
    climateControl.airConditioner.setTemperature(param.asInt());
}

// MANUAL PLANT WATERING
BLYNK_WRITE(PLNT_VPIN)
{
    DEBUG_PRINTLN("MANUAL: Plant watering started");
    manualWaterOverride = true;
}