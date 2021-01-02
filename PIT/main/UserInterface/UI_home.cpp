#include "Display/Display.h"
#include "Utilities.h"
#include "TemperatureSensing.h"
#include "UI.h"
#include "Persistance.h"

#include "LiquidCrystal_I2C.h"
#include <memory>

namespace PIT{

    extern uint64_t uptime_at_cycle_start;
    extern uint64_t uptime_at_pause;
    extern RUN_MODE run_mode;
    extern bool button_press_detected;
    extern uint32_t press_detection_time;

    void UI::displayStatusLine(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd, TemperatureSensing * sensor){
        
        auto sensor_state = sensor->getState();
        
        lcd.noBlink();
    
        lcd.setCursor(0, 0);
     
        uint64_t current_time = Utilities::getSystemUptime();
        uint64_t cycle_difference = (config.period - ((current_time - uptime_at_cycle_start) / 1000.0));
    
        String pw_pe = "";

        if(run_mode == 2)
        {
            if(!(config.enable_temperature_control && config.tmp_ctl_is_blocking && sensor_state->is_ready))
                pw_pe += Utilities::generateTimeString(cycle_difference, true, false, false);
        }
        else
            if(run_mode == 0)
            {
                if(!(config.enable_temperature_control && config.tmp_ctl_is_blocking && sensor_state->is_ready))
                    pw_pe += String("0");
            }
            else
                if(run_mode == 1)
                {
                    cycle_difference = (config.period - ((uptime_at_pause - uptime_at_cycle_start) / 1000.0));

                    pw_pe += Utilities::generateTimeString(cycle_difference, true, false, false);                
                }

        if(sensor_state->is_ready)
        {
            if(!(config.enable_temperature_control && config.tmp_ctl_is_blocking && sensor_state->is_ready))
            {
                pw_pe += " " + String(sensor_state->getLinRegTemperature(0), 1);
            }
            else
            {
                pw_pe += String(sensor_state->getLinRegTemperature(0), 2);

                pw_pe += String((char)0b11011111) + "F";
            }

            
            if(config.enable_temperature_control && config.tmp_ctl_is_blocking && sensor_state->is_ready)
            {
                float current_t = sensor_state->getLinRegTemperature(0);

                float t_in_1_m = sensor_state->getLinRegTemperature(60);
                
                float diff = t_in_1_m - current_t;

                if(diff < 0.0)
                    diff *= -1.0;

                if(diff < 0.025 || ((*(sensor_state->lrCoef))[0] == 0.0))
                    pw_pe += String((char)5);
                else
                if((*(sensor_state->lrCoef))[0] > 0.0)
                    pw_pe += String((char)6);
                else
                    pw_pe += String((char)7);
                
                pw_pe += diff >= 0.05 ? String(t_in_1_m - current_t, 3) : "0.0";
            }
        }

        int str_len = pw_pe.length();

        for(int i = 0; i < (16-1-str_len); i++)
            pw_pe += " ";

        lcd.setCursor(0, 0);

        lcd.print(pw_pe);

        lcd.setCursor(15, 0);
        
        switch(run_mode)
        {
            case 0:
                lcd.write((uint8_t)2);
                break;
            case 1:
                lcd.write((uint8_t)1); //pause
                break;
            case 2:

            if(config.enable_temperature_control && sensor_state->is_ready && config.tmp_ctl_is_blocking)
                lcd.write((uint8_t)3);
            else
                if(config.enable_temperature_control && sensor_state->is_ready)
                    lcd.write((uint8_t)4);
                else
                    lcd.write((uint8_t)0);
            break;
        } 

        delete sensor_state;
    }

    void UI::idleDisplay(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd, TemperatureSensing * sensor){

        int button_press = getButtonPress(); //check to see if we need to enter the menu system

        if(button_press == -1) //if not then update the idle display
        {
            std::unique_ptr<TemperatureSensing::SensorState> sensor_state(sensor->getState());
        
            lcd.setCursor(0, 1);
            
            if(config.enable_temperature_control && config.tmp_ctl_is_blocking){

                lcd.setCursor(0, 1);
                
                float range = config.setpoint_0 - sensor_state->getLinRegTemperature(0);

                lcd.print(range, 2);
                lcd.write((uint8_t)0b11011111);
                lcd.print("F ");

                if(TemperatureSensing::tcheck(&config, sensor_state.get()))
                    lcd.print("TRIG");
                else
                    lcd.print("NORM");

                Display::LCDPrint_P(lcd, Display::blank_line_str);                      
            }
            else{

                uint64_t current_uptime = Utilities::getSystemUptime();

                int num_blocks = round(((run_mode == 1 ? uptime_at_pause : current_uptime - uptime_at_cycle_start) / 1000.0 / config.period) * 16);
                int pulse_blocks = round(((float)config.pulse_width/config.period)*16);

                for(int i = 0; i < 16; i++)
                    if(i < num_blocks){

                        if(pulse_blocks == 0 && i == 0)
                            lcd.print('|');
                        else
                            if(i < pulse_blocks)
                                lcd.print((char)255);
                            else
                                lcd.print('_');
                    }
                    else
                        lcd.print(' ');
            }
        }
        else
            menuSelection(button_press, config, lcd);
    }
}