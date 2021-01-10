#include "PIT.h"
#include "Beep.h"
#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <OneWire.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>

#include "Utilities.h"
#include "Display/Display.h"
#include "Persistance.h"
#include "UserInterface/UI.h"
#include "TemperatureSensing.h"

#include "TimerCore.h"
#include <LiquidCrystal_I2C.h>
#include <memory>
#include <random>

 
namespace PIT{

    OneWire one_wire;
    TemperatureSensing * t_sense = NULL;

    uint32_t press_detection_time = 0;
    uint8_t button_press_detected = false;

    uint64_t system_uptime = 0;

    char run_mode = 0;

    uint64_t uptime_at_cycle_start = 0;
    uint64_t uptime_at_pause = 0;

    uint32_t button_block_timer = 0;

    uint8_t force_extern = 0;
    uint8_t t_chk_res = false;

    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

    void Timer_Task(void * pv_param){

        while(true){

            uint32_t tmr = millis();
            
            TimerCore::processTimer();
            
            while((uint32_t)((int32_t)millis()-tmr) < (1000/10))
                vTaskDelay(1);
        }
    }

    int ii = 0;
    int kk = 0;

    void UI_Task(void * pv_param){

        auto display = Display::getInstance();
        auto config = Persistance::getConfig();
        
        
        while(true){

            uint32_t tmr = millis();
            
            auto lcd = display.checkOut();

            UI::displayStatusLine(config, lcd, t_sense);
            UI::idleDisplay(config, lcd, t_sense);

            display.checkIn(lcd);

            while((uint32_t)((int32_t)millis()-tmr) < (1000/30))
                vTaskDelay(1); //give other tasks a chance to do something
        }
    }

    /**
     * Callback from button press interrupt
     */
    void buttonPressDetected(){

        if((uint32_t)((int32_t)millis()-button_block_timer) >= BUTTON_READ_BLOCK_DURATION){
            button_block_timer = millis();
            press_detection_time = millis();
            button_press_detected = true;
        }
        else
            button_block_timer = millis();
        
    }

    /**
     * Initalizes the system.
     * 
     * @param none
     * @return none
     */
    void init()
    {
        pinMode(BUTTON_PIN, INPUT_PULLUP);
        pinMode(RELAY_PIN, OUTPUT);
        pinMode(BEEP_PIN, OUTPUT);

        digitalWrite(RELAY_PIN, LOW);

        Beep::beep(10000);
        Beep::beep(10000);
        
        Serial.begin(SERIAL_RATE);

        EEPROM.begin(1024);
        
        auto config = Persistance::getConfig(); //initalize magic static
        auto display = Display::getInstance(); //initalize magic static

        auto lcd = display.checkOut();

        UI::showBootMessage(lcd); //default delay
        display.checkIn(lcd);

        pinMode(ONE_WIRE_PIN, OUTPUT);
        digitalWrite(ONE_WIRE_PIN, LOW);
        delay(100);
        one_wire.begin(ONE_WIRE_PIN);

        t_sense = new TemperatureSensing(&one_wire);
        t_sense->start();

        if(Persistance::getConfig().run_on_power_up)
            TimerCore::setMode(2);
        else
            TimerCore::setMode(0);

        attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPressDetected, FALLING);

        //create UI task
        xTaskCreatePinnedToCore(UI_Task, "uiTask", 8000, NULL, 1, NULL, 0);

        //create timer task
        xTaskCreatePinnedToCore(Timer_Task, "timerTask", 8000, NULL, 10, NULL, 0);

    }
}

