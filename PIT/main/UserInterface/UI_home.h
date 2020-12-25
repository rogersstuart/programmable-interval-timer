#include <Arduino.h>
#include "Display.h"
#include "Utilities.h"
#include "TemperatureSensing.h"
#include "UI.h"
#include "PITConfig.h"

namespace PIT{

    void UI::displayStatusLine(TemperatureSensor * sensor){

        auto config = PITConfig::getConfig();
        
        Display::displayHome(DISABLE_BLINK);
        
        uint64_t current_time = getSystemUptime();
        uint64_t cycle_difference = (active_config.period - ((current_time - uptime_at_cycle_start) / 1000.0));
    
        String pw_pe = "";

        if(run_mode == 2){

            if(!(active_config.enable_temperature_control && active_config.tmp_ctl_is_blocking && en_temp))
                pw_pe += Utilities.generateTimeString(cycle_difference, true, false, false);
        }
        else
            if(run_mode == 0){

                if(!(active_config.enable_temperature_control && active_config.tmp_ctl_is_blocking && en_temp))
                    pw_pe += String("0");
            }
            else
                if(run_mode == 1){

                    cycle_difference = (active_config.period - ((uptime_at_pause - uptime_at_cycle_start) / 1000.0));

                    pw_pe += Utilities.generateTimeString(cycle_difference, true, false, false);                
                }

        if(en_temp){

            if(!(active_config.enable_temperature_control && active_config.tmp_ctl_is_blocking && en_temp)){

                pw_pe += " " + String(getLinRegTemperature(0), 1);
            }
            else{

                pw_pe += String(getLinRegTemperature(0), 2);

                pw_pe += String((char)0b11011111) + "F";
            }

            
            if(active_config.enable_temperature_control && active_config.tmp_ctl_is_blocking && en_temp){

                float current_t = getLinRegTemperature(0);

                float t_in_1_m = getLinRegTemperature(60);
                
                float diff = t_in_1_m - current_t;

                if(diff < 0.0)
                    diff *= -1.0;

                if(diff < 0.025 || (lrCoef[0] == 0.0))
                    pw_pe += String((char)5);
                else
                if(lrCoef[0] > 0.0)
                    pw_pe += String((char)6);
                else
                    pw_pe += String((char)7);
                
                pw_pe += diff >= 0.05 ? String(t_in_1_m - current_t, 3) : "0.0";
            }
        }

        int str_len = pw_pe.length();

        for(int i = 0; i < (16-1-str_len); i++)
            pw_pe += " ";

        display.setCursor(0, 0);
        display.print(pw_pe);
        display.setCursor(15, 0);
        
        switch(run_mode){

            case 0:
            lcd.write((uint8_t)2);
            break;
            case 1:
            lcd.write((uint8_t)1); //pause
            break;
            case 2:

            if(active_config.enable_temperature_control && en_temp && active_config.tmp_ctl_is_blocking)
                display.write((uint8_t)3);
            else
                if(active_config.enable_temperature_control && en_temp)
                    display.write((uint8_t)4);
                else
                    display.write((uint8_t)0);
            break;
        } 
    }

    void UI::idleDisplay(){

        lcd.setCursor(0, 1);
        
        if(active_config.enable_temperature_control && active_config.tmp_ctl_is_blocking && en_temp){

            if(!lrcoef_is_valid)
                return;
            
            lcd.setCursor(0, 1);
            
            float range = active_config.setpoint_0 - getLinRegTemperature(0);

            lcd.print(range, 2);
            lcd.write((uint8_t)0b11011111);
            lcd.print("F ");

            if(tcheck())
                lcd.print("TRIG");
            else
                lcd.print("NORM");

            LCDPrint_P(blank_line_str);                      
        }
        else{

            uint64_t current_uptime = getSystemUptime();

            int num_blocks = round(((run_mode == 1 ? uptime_at_pause : current_uptime - uptime_at_cycle_start) / 1000.0 / active_config.period) * 16);
            int pulse_blocks = round(((float)active_config.pulse_width/active_config.period)*16);

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

}