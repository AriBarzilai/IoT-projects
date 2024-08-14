#include "env.h"
#include <BlynkSimpleEsp32.h>
#include <Arduino.h>

#include "climateHandler\climateControl.h"
#include "presenceDetection\tenantHandler.h"
#include "plantWateringSystem\plantWateringSystem.h"

#include <WiFi.h>
#include <WiFiClient.h>

#include <DHT.h>

// Temperature sensor
#define DHT_PIN 15
#define DHT_TYPE DHT22
#define UPDATE_INTERVAL_PLANT_DATA 2000

pState currState = DETECT_CLIMATE;

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
void switchToState(pState newState);
void setVirtualPin(int pin, int value);

void setup()
{

    Serial.begin(9600);
    
    
    climateControl = ClimateControl();
    plant = plantWateringSystem();

    plant.initialize();
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    dht.begin();
    tenantHandler.initTenants();
    
    switchToState(pState::DETECT_CLIMATE);
    PDEBUG_PRINTLN("BEGIN SMART HOME SYSTEM");
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
double aT , tempH ,tempT; // apparent temperature
void getRoomTemp()
{
    tempH = dht.readHumidity();
    tempT = dht.readTemperature();
    if (!isnan(tempH) && !isnan(tempT))
    {
        humidity = tempH;
        temperature = tempT;
        aT = 1.1 * temperature + 0.0261 * humidity - 3.944; // apparent (perceived) temperature. simple formula taken from online;
        Blynk.virtualWrite(V5, aT);

        PDEBUG_PRINTLN("CLIMATE DETECTION");
        PDEBUG_PRINT("Room temp: ");
        PDEBUG_PRINTLN(aT);
        PDEBUG_PRINTLN("PRESENCE DETECTION");

        switchToState(pState::DETECT_PRESENCE);
    }
}

long pingCooldown = PING_BEFORE_TENANTS;
long lastTenantPing = -pingCooldown;
void call_ping()
{
    if (!(millis() - lastTenantPing >= pingCooldown))
    {
        switchToState(pState::CONTROL_CLIMATE);
        return;
    }

    tenantHandler.pingNextTenant();
    if (tenantHandler.allTenantsPinged())
    {
        // switch to another state only after finished pinging all tenants
        pingCooldown = PING_BEFORE_TENANTS;
        lastTenantPing = millis();
        setVirtualPin(V4, tenantHandler.getHomeTenantsCount()); // update the number of tenants home in blynk
    }
}

long climateControlCooldown = 1200000L;
long lastClimateOverride = -climateControlCooldown;
void call_controlClimate()
{
    PDEBUG_PRINT("AUTO CLIMATE CONTROL");
    if (millis() - lastClimateOverride < climateControlCooldown)
    {
        PDEBUG_PRINTLN(" (OVERRIDEN)");
        switchToState(pState::CONTROL_PLANTS);
        return;
    }
    PDEBUG_PRINTLN("");

    climateControl.updateTemperature(aT);
    if (tenantHandler.getHomeTenantsCount() == 0)
    {
        climateControl.onHouseEmpty();
    }
    else
    {
        climateControl.handleClimate();
    }
    switchToState(pState::CONTROL_PLANTS);
}

int plantMoisture;
unsigned long lastUpdateData = 0;
void call_plantControl()
{
    // control the plants
    PDEBUG_PRINTLN("PLANT CONTROL");

    if (plant.needsWatering() || manualWaterOverride)
    {
        PDEBUG_PRINTLN("Watering plant");
        plant.startWatering();
        setVirtualPin(PLNT_DATA_VPIN, 1);
        manualWaterOverride = false;
    }

    if (!plant.isWatering())
    {
        if (millis() - lastUpdateData > UPDATE_INTERVAL_PLANT_DATA)
        {
            plantMoisture = map(plant.getMoistureValue(), 0, 4095, 0, 100);
            setVirtualPin(PLNT_DATA_VPIN, plantMoisture);
            setVirtualPin(PLNT_VPIN, 0);
            lastUpdateData = millis();
        }
        switchToState(pState::DETECT_CLIMATE);
    }
}

void switchToState(pState newState)
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

void setVirtualPin(int pin, int value)
{
    Blynk.virtualWrite(pin, value);
}

// POWER STATE (on/off)
BLYNK_WRITE(PWR_VPIN)
{
    PDEBUG_PRINT("MANUAL: ");
    lastClimateOverride = millis();
    climateControl.airConditioner.setPowerState((PowerState)param.asInt());
}

// TEMPERATURE MODE (cold/hot)
BLYNK_WRITE(TMP_MD_VPIN)
{
    PDEBUG_PRINT("MANUAL: ");
    lastClimateOverride = millis();
    climateControl.airConditioner.setTemperatureMode((TemperatureMode)param.asInt());
}

// FAN SPEED (1/2/3/auto)
BLYNK_WRITE(F_SPD_VPIN)
{
    PDEBUG_PRINT("MANUAL: ");
    lastClimateOverride = millis();
    climateControl.airConditioner.setFanSpeed((FanSpeed)param.asInt());
}

// TEMPERATURE (16-30)
BLYNK_WRITE(TMP_VPIN)
{
    PDEBUG_PRINT("MANUAL: ");
    lastClimateOverride = millis();
    climateControl.airConditioner.setTemperature(param.asInt());
}

// MANUAL PLANT WATERING
BLYNK_WRITE(PLNT_VPIN)
{
    PDEBUG_PRINTLN("MANUAL: Plant watering started");
    manualWaterOverride = true;
}