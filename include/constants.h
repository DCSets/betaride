#include "Arduino.h"

#ifndef CONSTANTS_H
#define CONSTANTS_H

const bool DEBUG = true;

constexpr const char *NVS_NAMESPACE = "config";
// Resources types
constexpr const char *TYPE_MOTORS = "m";
constexpr const char *TYPE_BRUSHLESS_MOTORS = "bm";
constexpr const char *TYPE_SERVOS = "s";
constexpr const char *TYPE_CONTROLLER = "c";
constexpr const char *TYPE_CONTROLLER_RULES = "cr";
// Resources limits
constexpr int MAX_MOTORS = 10;
constexpr int MAX_BRUSHLESS_MOTORS = 10;
constexpr int MAX_CONTROLLERS = 1;
constexpr int MAX_RULES = 20;


constexpr int CHANNELS_PRINT_RATE = 50;
// ELRS Channels output
constexpr int MIN_STICK_VALUE = 990;
constexpr int MAX_STICK_VALUE = 2011;
constexpr int SWITCH_LOW = 1000;
constexpr int SWITCH_MID = 1500;
constexpr int SWITCH_HIGH = 2000;

// ELRS default settings
constexpr int DEFAULT_PIN_RX = GPIO_NUM_16;
constexpr int DEFAULT_PIN_TX = GPIO_NUM_17;
constexpr int DEFAULT_RX_UART = 2;
constexpr int DEFAULT_THROTTLE_CHANNEL = 3;
constexpr int DEFAULT_DIRECTION_CHANNEL = 6;

// ESC default settings
constexpr int DEFAULT_ESC_FREQUENCY = 50;
constexpr int DEFAULT_ESC_RESOLUTION = 16;
constexpr int DEFAULT_ESC_CHANEL = 0;
constexpr int DEFAULT_ESC_PIN = GPIO_NUM_18;
constexpr bool DEFAULT_IS_3D = true;
constexpr int DEFAULT_THROTTLE_CENTER = 1488;
constexpr int DEFAULT_THROTTLE_MIN = 1148;
constexpr int DEFAULT_THROTTLE_MAX = 1832;
constexpr int DEFAULT_REVERSE_TIMEOUT = 500;

#endif