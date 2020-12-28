#include "Utilities.h"
#include "TemperatureSensing.h"
#include "UI.h"
#include "Persistance.h"
#include "PIT.h"

#define NUISANCE_PRESS -1
#define SHORT_PRESS 0
#define LONG_PRESS 1

namespace PIT{

    PRESS_TYPE UI::getButtonPress()
    {
        int8_t ret_val = -1;

        if(button_press_detected)
        {
            uint32_t press_detection_time_cpy = press_detection_time;
            
            delay(BUTTON_READ_BLOCK_DURATION);

            while(!digitalRead(BUTTON_PIN))
                if((uint32_t)((long)millis()-press_detection_time_cpy) >= LONG_PRESS_DURATION)
                    break;

            if((uint32_t)((long)millis()-press_detection_time_cpy) >= LONG_PRESS_DURATION)
                ret_val = 1;
            else
                if((uint32_t)((long)millis()-press_detection_time_cpy) >= SHORT_PRESS_DURATION)
                    ret_val = 0;

            delay(BUTTON_READ_BLOCK_DURATION);

            button_press_detected = false;
        }

        return ret_val;
    }

}
