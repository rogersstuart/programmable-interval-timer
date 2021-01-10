#ifndef PIT_BEEP_H
#define PIT_BEEP_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>

namespace PIT{

    class Beep{

        private:

            SemaphoreHandle_t handle;
            esp_timer_handle_t timer;
            esp_timer_create_args_t timer_args;

            Beep();
            static Beep& getInstance();

        public:

            static void beep(uint64_t time = 2000);
    };
}

#endif //PIT_BEEP_H