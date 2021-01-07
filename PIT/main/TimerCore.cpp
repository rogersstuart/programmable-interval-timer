#include "TimerCore.h"
#include <memory>
#include "Persistance.h"
#include "TemperatureSensing.h"
#include "PIT.h"
#include "Utilities.h"

namespace PIT{

    extern char run_mode;
    extern uint8_t t_chk_res;
    extern uint64_t uptime_at_cycle_start;
    extern uint64_t uptime_at_pause;
    extern uint8_t force_extern;
    extern TemperatureSensing* t_sense;

    /**
     * 
     */
    void TimerCore::processTimer(){

        auto config = Persistance::getConfig(); //isolated
        std::unique_ptr<TemperatureSensing::SensorState> sensor_state(t_sense->getState());
        
        if(run_mode == THERMOSTAT_MODE){

            t_chk_res = TemperatureSensing::tcheck(&config, sensor_state.get());
            
            uint64_t current_time = Utilities::getSystemUptime();
            float cycle_difference = (current_time - uptime_at_cycle_start) / 1000.0;

            if(cycle_difference >= config.period){

                uptime_at_cycle_start = current_time;
                cycle_difference = 0;
            }

            if(force_extern == 0){

                if(!(config.enable_temperature_control && config.tmp_ctl_is_blocking && sensor_state->is_ready)){

                    if((uint32_t)cycle_difference >= config.pulse_width && !t_chk_res)
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
    void TimerCore::setMode(RUN_MODE new_mode)
    {
        if(run_mode == new_mode)
            return;

        if((run_mode == 0 && new_mode == 2) || (run_mode == 0 && new_mode == 1))
        {
            run_mode = new_mode;
            uptime_at_cycle_start = Utilities::getSystemUptime();
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
            uint64_t new_uptime = Utilities::getSystemUptime();
            uint64_t offset = new_uptime - uptime_at_pause;
            uptime_at_cycle_start += offset;
            return;
        }

        if(run_mode == 2 && new_mode == 1)
        {
            run_mode = new_mode;
            uptime_at_pause = Utilities::getSystemUptime();
            return;  
        }
    }
}