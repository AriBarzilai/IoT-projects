#ifndef CONTROLLER_DATA_H
#define CONTROLLER_DATA_H

#include <stdint.h>
#include <array>

struct AcCommand
{
    uint16_t headerMark;
    uint16_t headerSpace;
    uint16_t bitMark;
    uint16_t oneSpace;
    uint16_t zeroSpace;
    uint64_t rawData[2];
};

constexpr size_t FAN_SPEEDS = 4;
constexpr size_t TEMPERATURES = 15;

extern const AcCommand ON_COLD_FAN1[TEMPERATURES];
extern const AcCommand ON_COLD_FAN2[TEMPERATURES];
extern const AcCommand ON_COLD_FAN3[TEMPERATURES];
extern const AcCommand ON_COLD_FANA[TEMPERATURES];

extern const AcCommand OFF_COLD_FAN1[TEMPERATURES];
extern const AcCommand OFF_COLD_FAN2[TEMPERATURES];
extern const AcCommand OFF_COLD_FAN3[TEMPERATURES];
extern const AcCommand OFF_COLD_FANA[TEMPERATURES];

extern const std::array<const AcCommand *, FAN_SPEEDS> ON_COLD;
extern const std::array<const AcCommand *, FAN_SPEEDS> OFF_COLD;

#endif