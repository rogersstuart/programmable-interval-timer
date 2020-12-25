#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <Arduino.h>
#include "ButtonTracker.h"
#include "PIT.h"

namespace PIT{

    ButtonTracker::ButtonTracker(int button_pin, ButtonCallback onChangeCallback) : button_pin{button_pin}, onChangeCallback{onChangeCallback}{

        pinMode(BUTTON_PIN, INPUT_PULLUP);
        cbAction_semaphore = xSemaphoreCreateBinary();
    }

    void ButtonTracker::start(){

        attachInterrupt(digitalPinToInterrupt(button_pin), [&](){buttonChangeISR();}, CHANGE);
        xTaskCreate([&](){callbackAction();}, "cbAct", 8000, NULL, 1, cbAction_handle);
    }

    void ButtonTracker::buttonChangeISR(){

        bool pin_state = digitalRead(BUTTON_PIN);
        if(button_press_detected && pin_state) //if the button has been released (HIGH) and the press hasn't been canceled
        {
            button_press_detected = false;
            
            if((uint32_t)((long)millis()-press_detection_time) >= LONG_PRESS_DURATION)
                press_result = LONG_PRESS;
            else
                if((uint32_t)((long)millis()-press_detection_time) >= SHORT_PRESS_DURATION)
                    press_result = SHORT_PRESS;
                else
                    press_result = NUISANCE_PRESS;

            xSemaphoreGiveFromISR(cbAction_semaphore, NULL);
        }
        else
            if(!pin_state) //if the button has been pressed (LOW)
            {
                button_block_timer = millis();
                press_detection_time = millis();
                button_press_detected = true;

                //todo: press without release detection?
            }
    }

    void ButtonTracker::callbackAction(){
        while(true)
            if(xSemaphoreTake(cbAction_semaphore, 0xFFFF) == pdTRUE){
                //it's time to perform the callback
                onChangeCallback(press_result);

                xSemaphoreGive(cbAction_semaphore); //release the semaphore
            }
    }

    void ButtonTracker::stop(){
        detachInterrupt(digitalPinToInterrupt(button_pin));
        vTaskDelete(cbAction_handle);
    }
}