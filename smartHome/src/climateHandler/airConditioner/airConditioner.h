#ifndef AIR_CONDITIONER_H
#define AIR_CONDITIONER_H

#include <stdint.h>
#include "controllerData.h"

enum class PowerState
{
    OFF,
    ON
};
enum class TemperatureMode
{
    COLD,
    HOT
};
enum class FanSpeed
{
    FAN_1 = 1,
    FAN_2 = 2,
    FAN_3 = 3,
    FAN_AUTO = 4
};

class AirConditioner
{
public:
    AirConditioner();
    void setFanSpeed(FanSpeed speed);
    void setTemperature(uint8_t temp);
    void setPowerState(PowerState state);
    void setTemperatureMode(TemperatureMode mode);

    PowerState getPowerState() const { return powerState; }
    TemperatureMode getTemperatureMode() const { return temperatureMode; }
    FanSpeed getFanSpeed() const { return fanSpeed; }
    uint8_t getTemperature() const { return temperature; }

private:
    PowerState powerState;
    TemperatureMode temperatureMode;
    FanSpeed fanSpeed;
    uint8_t temperature;

    void transmitCurrentState();
    void transmitIRCommand(const AcCommand &command);
};

#endif // AIR_CONDITIONER_H