#include "PIT.h"
#include "Display.h"
#include "Utilities.h"
#include "TemperatureSensing.h"
#include "UI.h"
#include "Persistance.h"

#include "LiquidCrystal_I2C.h"

namespace PIT{

    void UI::displayStatusLine(TemperatureSensing * sensor){
        
        auto display = Display::getInstance();
        auto lcd = display.checkOut();
        auto config = Persistance::getConfig();
        
        Display::displayHome(DISABLE_BLINK);
        
        uint64_t current_time = Utilities::getSystemUptime();
        uint64_t cycle_difference = (config.period - ((current_time - uptime_at_cycle_start) / 1000.0));
    
        String pw_pe = "";

        if(run_mode == THERMOSTAT_MODE){

            if(!(config.enable_temperature_control && config.tmp_ctl_is_blocking)) //if thermostat control is enabled and isn't blocking
                pw_pe += Utilities.generateTimeString(cycle_difference, true, false, false);
        }
        else
            if(run_mode == 0){

                if(!(config.enable_temperature_control && config.tmp_ctl_is_blocking))
                    pw_pe += String("0");
            }
            else
                if(run_mode == 1){

                    cycle_difference = (config.period - ((uptime_at_pause - uptime_at_cycle_start) / 1000.0));

                    pw_pe += Utilities.generateTimeString(cycle_difference, true, false, false);                
                }

            if(!(config.enable_temperature_control && config.tmp_ctl_is_blocking)){

                pw_pe += " " + String(sensor->getLinRegTemperature(0), 1);
            }
            else{

                pw_pe += String(sensor->getLinRegTemperature(0), 2);

                pw_pe += String((char)0b11011111) + "F";
            }

            
        if(config.enable_temperature_control && config.tmp_ctl_is_blocking){

            //generate blocking temperature control status line

            float current_t = sensor->getLinRegTemperature(0);
            float t_in_1_m = sensor->getLinRegTemperature(60);
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

        //fill the rest of the length of the string so old characters are covered up
        int str_len = pw_pe.length();
        for(int i = 0; i < (16-1-str_len); i++)
            pw_pe += " ";

        //print the status line and mode character

        lcd.setCursor(0, 0);
        lcd.print(pw_pe);
        lcd.setCursor(15, 0);
        
        switch(run_mode){

            case 0:
                lcd.write((uint8_t)2);
                break;
            case 1:
                lcd.write((uint8_t)1); //pause
                break;
            case 2:
                if(config.enable_temperature_control && en_temp && config.tmp_ctl_is_blocking)
                    lcd.write((uint8_t)3);
                else
                    if(config.enable_temperature_control && en_temp)
                        lcd.write((uint8_t)4);
                    else
                        lcd.write((uint8_t)0);
                 break;
        }

        display.checkIn(lcd);
    }

    void UI::idleDisplay(){

        auto display = Display::getInstance();
        auto lcd = display.checkOut();
        auto config = Persistance::getConfig();
        
        lcd.setCursor(0, 1);
        
        if(config.enable_temperature_control && config.tmp_ctl_is_blocking){

            lcd.setCursor(0, 1);
            
            float range = config.setpoint_0 - sensor->getLinRegTemperature(0);

            lcd.print(range, 2);
            lcd.write((uint8_t)0b11011111);
            lcd.print("F ");

            if(tcheck())
                lcd.print("TRIG");
            else
                lcd.print("NORM");

            display.LCDPrint_P(Display::blank_line_str);                      
        }
        else{

            uint64_t current_uptime = Utilities::getSystemUptime();

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

        display.checkIn(lcd);
    }

}