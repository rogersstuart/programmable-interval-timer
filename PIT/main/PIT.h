#ifndef PIT_H
#define PIT_H

#ifndef ARDUINO
#define ARDUINO 101
#endif //ARDUINO

#define INCLUDE_vTaskDelete 1
#define configSUPPORT_DYNAMIC_ALLOCATION 1
#define INCLUDE_vTaskSuspend 1

#define SERIAL_RATE 115200U

#define BUTTON_PIN 16
#define RELAY_PIN 4
#define ONE_WIRE_PIN 15
#define BEEP_PIN 27

#define BRINGUP_CODE 0xCFB3

#define SECONDS_IN_DAY 86400ULL
#define SECONDS_IN_HOUR 3600ULL
#define SECONDS_IN_MINUTE 60ULL

#define TIMER_MODE 0
#define THERMOSTAT_MODE 2

typedef char RUN_MODE;

#endif //PIT_H