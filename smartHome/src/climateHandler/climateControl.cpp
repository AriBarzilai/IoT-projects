#include "climateControl.h"
#include "../env.h"
#include <BlynkSimpleEsp32.h>

ClimateControl::ClimateControl()
{
    airConditioner = AirConditioner();
}

void ClimateControl::updateTemperature(double temp)
{
    currTemp = temp;
}

void ClimateControl::onHouseEmpty()
{
    if (airConditioner.getPowerState() == PowerState::ON) // turn off AC
    {
        Blynk.virtualWrite(PWR_VPIN, (int)PowerState::OFF);
        airConditioner.setPowerState(PowerState::OFF);
    }
}

void ClimateControl::onColdMode()
{
    if (currTemp <= 10) // switch to heat mode
    {
        Blynk.virtualWrite(TMP_MD_VPIN, (int)TemperatureMode::HOT);
        airConditioner.setTemperatureMode(TemperatureMode::HOT);
    }
    else if (currTemp <= 18) // turn off the AC
    {
        Blynk.virtualWrite(PWR_VPIN, (int)PowerState::OFF);
        airConditioner.setPowerState(PowerState::OFF);
    }
    else
    {
        DEBUG_PRINTLN("AC cooling still necessary");
    }
}

void ClimateControl::onHeatMode()
{
    if (currTemp >= 27) // switch to cold mode
    {
        Blynk.virtualWrite(TMP_MD_VPIN, (int)TemperatureMode::COLD);
        airConditioner.setTemperatureMode(TemperatureMode::COLD);
    }
    else if (currTemp >= 22) // turn off the AC
    {
        Blynk.virtualWrite(PWR_VPIN, (int)PowerState::OFF);
        airConditioner.setPowerState(PowerState::OFF);
    }
    else
    {
        DEBUG_PRINTLN("AC heating still necessary");
    }
}

void ClimateControl::handleClimate()
{
    if (airConditioner.getPowerState() == PowerState::ON)
    {
        if (airConditioner.getTemperatureMode() == TemperatureMode::COLD)
        {
            onHeatMode();
        }
        else
        {
            onColdMode();
        }
    }
    else
    {
        if (currTemp <= 15) // turn on AC
        {
            setHotClimate();
        }
        else if (currTemp >= 25)
        {
            setColdClimate();
        }
    }
}

void ClimateControl::setHotClimate()
{
    Blynk.virtualWrite(PWR_VPIN, (int)PowerState::ON);
    Blynk.virtualWrite(TMP_MD_VPIN, (int)TemperatureMode::HOT);
    Blynk.virtualWrite(TMP_VPIN, 24);
    airConditioner.setPowerState(PowerState::ON);
    airConditioner.setTemperatureMode(TemperatureMode::HOT);
    airConditioner.setTemperature(24);
}

void ClimateControl::setColdClimate()
{
    Blynk.virtualWrite(PWR_VPIN, (int)PowerState::ON);
    Blynk.virtualWrite(TMP_MD_VPIN, (int)TemperatureMode::COLD);
    Blynk.virtualWrite(TMP_VPIN, 16);
    airConditioner.setPowerState(PowerState::ON);
    airConditioner.setTemperatureMode(TemperatureMode::COLD);
    airConditioner.setTemperature(16);
}
