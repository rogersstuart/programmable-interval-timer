#ifndef PIT_H
#define PIT_H

#define ARDUINO 100

//#include <TimerOne.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <EEPROM.h>
#include <DallasTemperature.h>
//#include <MsTimer2.h>
//#include <Poly.h>
#include "CustomChars.h"
#include <Arduino.h>
//

#define BUTTON_PIN 2
#define POT_PIN A1
#define RELAY_PIN 4
#define NUM_POT_AVG_ELEMENTS 32
#define MAX_POT_VALUE 1024

#define SHORT_PRESS_DURATION 100
#define LONG_PRESS_DURATION 1000
#define NUISANCE_PRESS_DURATION 10000

#define BUTTON_READ_BLOCK_DURATION 1000/30

#define BRINGUP_CODE 0xCFB3


#define SECONDS_IN_DAY 86400ULL
#define SECONDS_IN_HOUR 3600ULL
#define SECONDS_IN_MINUTE 60ULL

const char stop_pause_str[] PROGMEM = " Stop     Pause ";
const char stop_run_str[] PROGMEM = " Stop       Run ";
const char run_str[] PROGMEM = "       Run      ";
const char yes_no_str[] PROGMEM = "  Yes       No  ";
const char match_less_set_str[] PROGMEM = "Match t < set   ";

const char blank_line_str[] PROGMEM = "                ";

struct config
{
    uint64_t pulse_width = 60;
    uint64_t period = 120;
    uint8_t run_on_power_up = true;
    uint8_t enable_temperature_control = false;
    float setpoint_0 = 78.0f;
    uint8_t cmp_options = 0; //0 is less than, 1 is greater than
    uint8_t tmp_ctl_is_blocking = false;
} active_config;

#endif //PIT_H