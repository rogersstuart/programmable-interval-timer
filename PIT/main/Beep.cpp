#include "PIT.h"
#include "Beep.h"
#include <Arduino.h>

namespace PIT{

    Beep::Beep(){

        timer_args = esp_timer_create_args_t{
                .callback = (esp_timer_cb_t)([](void*){
                    //do something
                    digitalWrite(BEEP_PIN, LOW);
                }),
                .arg = (void*) timer,
                .name = "one-shot"
        };

        esp_timer_create(&timer_args, &timer);
    }

    Beep& Beep::getInstance(){

        static Beep beep;
        return beep;
    }

    void Beep::beep(uint64_t time){

        auto instance = getInstance();
        
        digitalWrite(BEEP_PIN, HIGH);
        esp_timer_start_once(instance.timer, time);
    }  
}