#include "PIT.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>


#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <OneWire.h>
#include "Utilities.h"
#include "Display/Display.h"
#include "ButtonTracker.h"
#include "Persistance.h"

#include "UserInterface/UI.h"

#include <vector>
#include <functional>
//

namespace PIT{

    ButtonTracker * button;
    
    OneWire oneWire(ONE_WIRE_PIN);
    _lock_t oneWireLock;

    uint32_t press_detection_time = 0;
    uint8_t button_press_detected = false;

    uint64_t system_uptime = 0;

    uint8_t run_mode = 0;

    uint64_t uptime_at_cycle_start = 0;
    uint64_t uptime_at_pause = 0;

    uint32_t button_block_timer = 0;

    uint8_t en_temp = false;

    uint8_t force_extern = 0;
    uint8_t t_chk_res = false;

    //vector<function<void(PRESS_TYPE)>>

    void buttonCallback(PRESS_TYPE press_type){

        switch(press_type){

            case SHORT_PRESS: return;
            case LONG_PRESS: return;
            case NUISANCE_PRESS: return;
            default:
        }
    }

    void processSerial(){

        uint8_t cfg_byte = Serial.read();
        Serial.write(0b11111010);

        if(cfg_byte & 1)
            force_extern = cfg_byte & 3;
        else
            force_extern = 0;
    }

    /**
     * 
     */
    void processTimer(){

        if(run_mode == 2){

            t_chk_res = tcheck();
            
            uint64_t current_time = getSystemUptime();
            float cycle_difference = (current_time - uptime_at_cycle_start) / 1000.0;

            if(cycle_difference >= active_config.period){

                uptime_at_cycle_start = current_time;
                cycle_difference = 0;
            }

            if(force_extern == 0){

                if(!(active_config.enable_temperature_control && active_config.tmp_ctl_is_blocking && en_temp)){

                    if((uint32_t)cycle_difference >= active_config.pulse_width && !t_chk_res)
                        pinMode(RELAY_PIN, INPUT); //off
                    else
                        pinMode(RELAY_PIN, OUTPUT); //on
                }
                else{

                    if(t_chk_res)
                        pinMode(RELAY_PIN, OUTPUT); //on 
                    else
                        pinMode(RELAY_PIN, INPUT); //off    
                } 
            }
            else{

                if(force_extern >> 1)
                    pinMode(RELAY_PIN, OUTPUT); //on
                else
                    pinMode(RELAY_PIN, INPUT); //off
            }  
        }
        else
            if(run_mode == 0)
                pinMode(RELAY_PIN, INPUT);    
    }

    /**
     * Sets the current opperational mode of the timer.
     */
    void setMode(RUN_MODE new_mode)
    {
        if(run_mode == new_mode)
            return;

        if((run_mode == 0 && new_mode == 2) || (run_mode == 0 && new_mode == 1))
        {
            run_mode = new_mode;
            uptime_at_cycle_start = getSystemUptime();
            return;
        }

        if((run_mode == 2 || run_mode == 1) && new_mode == 0)
        {
            run_mode = new_mode;
            uptime_at_cycle_start = 0;
            return;
        }

        if(run_mode == 1 && new_mode == 2)
        {
            run_mode = new_mode;
            uint64_t new_uptime = getSystemUptime();
            uint64_t offset = new_uptime - uptime_at_pause;
            uptime_at_cycle_start += offset;
            return;
        }

        if(run_mode == 2 && new_mode == 1)
        {
            run_mode = new_mode;
            uptime_at_pause = getSystemUptime();
            return;  
        }
    }

    void loop()
    {
        //refresh system uptime
        Utilities::getSystemUptime();

        UI::displayStatusLine();
        UI::idleDisplay();

        /*
        if(Serial.available() >= 2)
            if(Serial.read() == 0b10101111)
                processSerial();
        */

        vTaskDelay(10); //give other tasks a chance to do something
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

        pinMode(A0, INPUT_PULLUP);
        pinMode(A2, INPUT_PULLUP);
        pinMode(A3, INPUT_PULLUP);
        
        Persistance.getConfig(); //initalize magic static
        Display::getInstance(); //initalize magic static

        UI::showBootMessage(); //default delay

        if(Persistance::getConfig().run_on_power_up)
            setMode(2);
        else
            setMode(0);

        button = new Button(BUTTON_PIN, [](){buttonCallback();});
        button->start(); //begin listening for button presses

        while(true)
            loop();
    }
}

