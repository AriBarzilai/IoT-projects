#include "climateControl.h"
#include "../env.h"

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
        setVirtualPin(PWR_VPIN, (int)PowerState::OFF);
        airConditioner.setPowerState(PowerState::OFF);
    }
}

void ClimateControl::onColdMode()
{
    if (currTemp <= 10) // switch to heat mode
    {
        setVirtualPin(TMP_MD_VPIN, (int)TemperatureMode::HOT);
        airConditioner.setTemperatureMode(TemperatureMode::HOT);
    }
    else if (currTemp <= 18) // turn off the AC
    {
        setVirtualPin(PWR_VPIN, (int)PowerState::OFF);
        airConditioner.setPowerState(PowerState::OFF);
    }
    else
    {
        PDEBUG_PRINTLN("AC cooling still necessary");
    }
}

void ClimateControl::onHeatMode()
{
    if (currTemp >= 27) // switch to cold mode
    {
        setVirtualPin(TMP_MD_VPIN, (int)TemperatureMode::COLD);
        airConditioner.setTemperatureMode(TemperatureMode::COLD);
    }
    else if (currTemp >= 22) // turn off the AC
    {
        setVirtualPin(PWR_VPIN, (int)PowerState::OFF);
        airConditioner.setPowerState(PowerState::OFF);
    }
    else
    {
        PDEBUG_PRINTLN("AC heating still necessary");
    }
}

void ClimateControl::handleClimate()
{
    if (airConditioner.getPowerState() == PowerState::ON)
    {
        if (airConditioner.getTemperatureMode() == TemperatureMode::COLD)
        {
            onColdMode();
        }
        else
        {
            onHeatMode();
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
    setVirtualPin(PWR_VPIN, (int)PowerState::ON);
    setVirtualPin(TMP_MD_VPIN, (int)TemperatureMode::HOT);
    setVirtualPin(TMP_VPIN, 24);
    airConditioner.setPowerState(PowerState::ON);
    airConditioner.setTemperatureMode(TemperatureMode::HOT);
    airConditioner.setTemperature(24);
}

void ClimateControl::setColdClimate()
{
    setVirtualPin(PWR_VPIN, (int)PowerState::ON);
    setVirtualPin(TMP_MD_VPIN, (int)TemperatureMode::COLD);
    setVirtualPin(TMP_VPIN, 16);
    airConditioner.setPowerState(PowerState::ON);
    airConditioner.setTemperatureMode(TemperatureMode::COLD);
    airConditioner.setTemperature(16);
}
