#include "electraRemote.h"
#include <IRremote.hpp>

#define IR_SEND_PIN 4

constexpr uint8_t MIN_TEMPERATURE = 16;
constexpr uint8_t MAX_TEMPERATURE = 30;
uint64_t rawDataCopy[2];

ElectraRemote::ElectraRemote()
    : powerState(PowerState::OFF), temperatureMode(TemperatureMode::COLD),
      fanSpeed(FanSpeed::FAN_1), temperature(MIN_TEMPERATURE)
{
    IrSender.begin(IR_SEND_PIN);
}

void ElectraRemote::setFanSpeed(FanSpeed speed)
{
    if (speed >= FanSpeed::FAN_1 && speed <= FanSpeed::FAN_AUTO)
    {
        fanSpeed = speed;
        transmitCurrentState();
    }
}

void ElectraRemote::setTemperature(uint8_t temp)
{
    if (temp >= MIN_TEMPERATURE && temp <= MAX_TEMPERATURE)
    {
        temperature = temp;
        transmitCurrentState();
    }
}

void ElectraRemote::setPowerState(PowerState state)
{
    powerState = state;
    transmitCurrentState();
}

void ElectraRemote::setTemperatureMode(TemperatureMode mode)
{
    temperatureMode = mode;
    transmitCurrentState();
}

void ElectraRemote::transmitCurrentState()
{
    uint8_t adjustedTemperature = temperature - MIN_TEMPERATURE;
    uint8_t adjustedFanSpeed = static_cast<uint8_t>(fanSpeed) - 1;

    const AcCommand *commandArray = (powerState == PowerState::ON && temperatureMode == TemperatureMode::COLD)
                                        ? ON_COLD[adjustedFanSpeed]
                                        : OFF_COLD[adjustedFanSpeed];

    const AcCommand &command = commandArray[adjustedTemperature];

    transmitIRCommand(command);
}

void ElectraRemote::transmitIRCommand(const AcCommand &command)
{
    uint64_t rawDataCopy[2];
    rawDataCopy[0] = command.rawData[0];
    rawDataCopy[1] = command.rawData[1];

    IrSender.sendPulseDistanceWidthFromArray(
        38,
        command.headerMark,
        command.headerSpace,
        command.bitMark,
        command.oneSpace,
        command.bitMark,
        command.zeroSpace,
        rawDataCopy,
        104,
        PROTOCOL_IS_LSB_FIRST,
        0,
        0
    );

   
}
