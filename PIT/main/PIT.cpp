#include "PIT.h"
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
 
namespace PIT{

    OneWire one_wire(ONE_WIRE_PIN);
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

    void Timer_Task(void * pv_param){

        while(true){
            
            TimerCore::processTimer();
            vTaskDelay(10);
        }
    }

    void UI_Task(void * pv_param){

        auto display = Display::getInstance();
        auto config = Persistance::getConfig();
        
        while(true){

            auto lcd = display.checkOut();
            
            UI::displayStatusLine(config, lcd, t_sense);
            UI::idleDisplay(config, lcd, t_sense);

            display.checkIn(lcd);

            vTaskDelay(20); //give other tasks a chance to do something
        }
    }

    /**
     * Callback from button press interrupt
     */
    void buttonPressDetected(){

        button_block_timer = millis();
        press_detection_time = millis();
        button_press_detected = true;
    }

    /**
     * Initalizes the system.
     * 
     * @param none
     * @return none
     */
    void init()
    {
        Serial.begin(SERIAL_RATE);
        
        auto config = Persistance::getConfig(); //initalize magic static
        auto display = Display::getInstance(); //initalize magic static

        auto lcd = display.checkOut();
        UI::showBootMessage(lcd); //default delay
        display.checkIn(lcd);

        if(Persistance::getConfig().run_on_power_up)
            TimerCore::setMode(2);
        else
            TimerCore::setMode(0);

        attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPressDetected, FALLING);

        t_sense = new TemperatureSensing(&one_wire);

        //create UI task
        xTaskCreate(UI_Task, "uiTask", 8000, NULL, 1, NULL);

        //create timer task
        xTaskCreate(Timer_Task, "timerTask", 8000, NULL, 1, NULL);
    }
}

