#include "Arduino.h"
#include "Utilities.h"
#include "TemperatureSensing.h"
#include "UI.h"
#include "Persistance.h"
#include "PIT.h"

namespace PIT{

    extern uint64_t uptime_at_cycle_start;
    extern uint64_t uptime_at_pause;
    extern RUN_MODE run_mode;
    extern bool button_press_detected;
    extern uint32_t press_detection_time;
    extern uint32_t button_block_timer;

    PRESS_TYPE UI::getButtonPress()
    {
        PRESS_TYPE ret_val = -1;

        if(button_press_detected)
        {
            uint32_t press_detection_time_cpy = press_detection_time;
            
            //vTaskDelay(BUTTON_READ_BLOCK_DURATION);

            while(!digitalRead(BUTTON_PIN))
                if((uint32_t)((int32_t)millis()-press_detection_time_cpy) >= LONG_PRESS_DURATION)
                    break;

            if((uint32_t)((int32_t)millis()-press_detection_time_cpy) >= LONG_PRESS_DURATION)
                ret_val = 1;
            else
                if((uint32_t)((int32_t)millis()-press_detection_time_cpy) >= SHORT_PRESS_DURATION)
                    ret_val = 0;

            //vTaskDelay(BUTTON_READ_BLOCK_DURATION);

            button_block_timer = millis();
            button_press_detected = false;
            
        }

        return ret_val;
    }

}
