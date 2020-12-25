#ifndef PIT_CONFIG_H
#define PIT_CONFIG_H

namespace PIT{

    namespace{
        class lock_t;
    }

    class Persistance final {
        
        public:

            struct PITConfig
            {
                uint64_t pulse_width = 60;
                uint64_t period = 120;
                uint8_t run_on_power_up = true;
                uint8_t enable_temperature_control = false;
                float setpoint_0 = 78.0f;
                uint8_t cmp_options = 0; //0 is less than, 1 is greater than
                uint8_t tmp_ctl_is_blocking = false;
            };

            static PITConfig getConfig();
            static void setConfig(PITConfig config);

        private:
        
            Persistance() = default;

            inline static PITConfig config {};

            inline static lock_t cfg_lock;
            inline static bool init_complete;

            static void readConfig();
            static void writeConfig();

    };
}

#endif //PIT_CONFIG_H