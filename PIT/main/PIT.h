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