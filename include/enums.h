#include "Arduino.h"

#ifndef ENUMS_H
#define ENUMS_H

enum ControllerType
{
    UNKNOWN = 0,
    ELRS = 1,
    PS5 = 2
};

enum class MotorDirection
{
    Forward = 1,
    Backward = 2
};

// ===== ELRS Rules Types =====
enum class ChannelFunction
{
    FULL = 1,
    RANGE = 2,
    EXACT = 3,
    ABOVE = 4,
    BELOW = 5,
    ABOVE_OR_EQUAL = 6,
    BELOW_OR_EQUAL = 7,
};

enum class MotorFunction
{
    SPEED_PERCENT = 1,
    SPEED_RANGE = 2,
    SPEED_FULL = 3,
};

#endif