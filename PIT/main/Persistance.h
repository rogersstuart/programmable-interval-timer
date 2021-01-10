#ifndef PIT_CONFIG_H
#define PIT_CONFIG_H

#include <Arduino.h>

namespace PIT{

    class Persistance final {
        
        public:

            struct PITConfig
            {
                uint64_t pulse_width = 60;
                uint64_t period = 120;
                bool run_on_power_up = true;
                bool enable_temperature_control = false;
                float setpoint_0 = 78.0f;
                uint8_t cmp_options = 0; //0 is less than, 1 is greater than
                bool tmp_ctl_is_blocking = true;
            };

            static PITConfig getConfig();
            static void setConfig(PITConfig config);

        private:

            SemaphoreHandle_t cfg_lock;
            
            PITConfig config {};
            bool init_complete;

            void readConfig();
            void writeConfig();
            
            Persistance();

            static Persistance& getInstance();
    };
}

#endif //PIT_CONFIG_H