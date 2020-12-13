#ifndef PIT_H
#define PIT_H

#define ARDUINO 100

#define INCLUDE_vTaskDelete 1

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>

//#include <TimerOne.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
//#include <Poly.h>
#include <Arduino.h>
#include "DisplayManager.h"

//

#define SERIAL_RATE 250000U

#define BUTTON_PIN 2
#define RELAY_PIN 4

#define SHORT_PRESS_DURATION 100
#define LONG_PRESS_DURATION 1000
#define NUISANCE_PRESS_DURATION 10000

#define BUTTON_READ_BLOCK_DURATION 1000/30

#define BRINGUP_CODE 0xCFB3

#define SECONDS_IN_DAY 86400ULL
#define SECONDS_IN_HOUR 3600ULL
#define SECONDS_IN_MINUTE 60ULL

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

OneWire oneWire(10);
DisplayManger display;

#endif //PIT_H