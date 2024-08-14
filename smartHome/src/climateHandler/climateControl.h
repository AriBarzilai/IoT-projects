#ifndef CLIMATE_CONTROL_H
#define CLIMATE_CONTROL_H

#include "airConditioner/airConditioner.h"
#include "env.h"

extern void setVirtualPin(int pin, int value);

class ClimateControl
{
public:
    ClimateControl();
    AirConditioner airConditioner;
    void updateTemperature(double temp);
    void onHouseEmpty();
    void handleClimate();

private:
    double currTemp;
    void onColdMode();
    void onHeatMode();
    void setColdClimate();
    void setHotClimate();
};

#endif // CLIMATE_CONTROL_H