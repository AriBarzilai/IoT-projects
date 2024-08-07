#ifndef ELECTRA_REMOTE_H
#define ELECTRA_REMOTE_H

#include <stdint.h>
#include "electra_remote_data.h"

enum class PowerState { OFF, ON };
enum class TemperatureMode { COLD, HOT };
enum class FanSpeed { FAN_1 = 1, FAN_2 = 2, FAN_3 = 3, FAN_AUTO = 4 };

class ElectraRemote {
public:
    ElectraRemote();
    void setFanSpeed(FanSpeed speed);
    void setTemperature(uint8_t temp);
    void setPowerState(PowerState state);
    void setTemperatureMode(TemperatureMode mode);

private:
    PowerState powerState;
    TemperatureMode temperatureMode;
    FanSpeed fanSpeed;
    uint8_t temperature;

    void transmitCurrentState();
    void transmitIRCommand(const AcCommand& command);
};

#endif // ELECTRA_REMOTE_H