#include "PIT.h"
#include <Arduino.h>
#include "Persistance.h"
#include "Display/Display.h"
#include "UI.h"
#include <LiquidCrystal_I2C.h>

#include "Utilities.h"

#include "TimerCore.h"
#include "RotaryEncoder.h"



namespace PIT{

    extern uint64_t uptime_at_cycle_start;
    extern uint64_t uptime_at_pause;
    extern RUN_MODE run_mode;
    extern bool button_press_detected;
    extern uint32_t press_detection_time;

    //using PITConfig = PIT::Persistance::PITConfig;

    void UI::menuSelection(PRESS_TYPE button_press, Persistance::PITConfig& config, LiquidCrystal_I2C& lcd){

        lcd.setCursor(0, 1);

        if(button_press == 0)
        {
            if(run_mode == 2)
                Display::LCDPrint_P(lcd, Display::stop_pause_str);
            else
                if(run_mode == 1)
                    Display::LCDPrint_P(lcd, Display::stop_run_str);
                else
                    if(run_mode == 0)
                        Display::LCDPrint_P(lcd, Display::run_str);                           

            uint16_t pot_value = 0; //encoder default positon

            //reset the state of the encoder tracker
            encoder.setBoundaries(0, 1, true);
            encoder.reset();

            //while(getButtonPress() != )

            switch(pot_value)
            {
                case 0: switch(run_mode){

                            case 2: lcd.setCursor(1, 1); break;
                            case 1: lcd.setCursor(1, 1); break;
                            case 0: lcd.setCursor(7, 1); break;
                        }

                        break;

                case 1: switch(run_mode){

                            case 2: lcd.setCursor(10 , 1); break;
                            case 1: lcd.setCursor(12 , 1); break;
                            case 0: lcd.setCursor(7 , 1); break;
                        }

                        break;
            }
                                
            lcd.blink();
            
            uint32_t timer = millis();
            while((uint32_t)((long)millis()-timer) < NUISANCE_PRESS_DURATION){

                //pot_value = getscaledPotValue(2);
                pot_value = encoder.readEncoder();

                switch(pot_value){

                    case 0: switch(run_mode)
                            {
                                case 2: lcd.setCursor(1 , 1); break;
                                case 1: lcd.setCursor(1 , 1);break;
                                case 0: lcd.setCursor(7 , 1);break;
                            }
                            break;

                    case 1: switch(run_mode)
                            {
                                case 2: lcd.setCursor(10 , 1); break;
                                case 1: lcd.setCursor(12 , 1);break;
                                case 0: lcd.setCursor(7 , 1);break;
                            }
                            break;
                }

                //delay(100);

                switch(getButtonPress())
                {
                    case -2: //lcd.print("Nuisance Press");
                            break;

                    case -1: //lcd.print("No Press");
                            break;

                    case 0: //config.pulse_width = (uint64_t)pot_value;
                            //writeConfig();
                            //lcd.print(F("Short Press"));
                            //delay(1000);
                            return;

                    case 1: if(run_mode == 2)
                                Display::LCDPrint_P(lcd, Display::stop_pause_str);
                            else
                                if(run_mode == 1)
                                    Display::LCDPrint_P(lcd, Display::stop_run_str);
                                else
                                    if(run_mode == 0)
                                        Display::LCDPrint_P(lcd, Display::run_str);
                            
                            switch(pot_value)
                            {
                                case 0: switch(run_mode)
                                        {
                                            case 2: TimerCore::setMode(0); break;
                                            case 1: TimerCore::setMode(0);
                                            case 0: TimerCore::setMode(2); break;
                                        }
                                        break;

                                case 1: switch(run_mode)
                                        {
                                            case 2: TimerCore::setMode(1); break;
                                            case 1: TimerCore::setMode(2); break;
                                            case 0: TimerCore::setMode(2); break;
                                        }
                                        break;
                            }
                            return;
                }
            }
        }
        else
        if(button_press == 1)
        {
            selectOption(config, lcd);
        }    
    }

    void UI::selectOption(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd){

        int menu_position = 0;

        switch(menu_position){

            case 0: selectOptionMenuItem1(config, lcd);
            break;
            case 1: selectOptionMenuItem2(config, lcd);
            break;
            case 2: selecOptionMenuItem3(config, lcd);
            break;
        }

        uint32_t timer = millis();
        while((uint32_t)((long)millis()-timer) < NUISANCE_PRESS_DURATION){

            switch(getButtonPress())
            {
                case 0:

                    if(++menu_position == 3)
                        menu_position = 0;

                    switch(menu_position){

                        case 0: selectOptionMenuItem1(config, lcd);
                        break;
                        case 1: selectOptionMenuItem2(config, lcd);
                        break;
                        case 2: selecOptionMenuItem3(config, lcd);
                    }

                    break;

                case 1:

                    switch(menu_position){

                        case 0: setPulseWidthMenu(config, lcd);
                        break;
                        case 1: setPeriodWidthMenu(config, lcd);
                        break;
                        case 2: temperatureOptionsMenu(config, lcd);
                        break;
                    }
                
                    return;
            }
        }
    }

    void UI::selectOptionMenuItem1(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd){

        lcd.noBlink();
        
        lcd.setCursor(0, 1);
        Display::LCDPrint_P(lcd, Display::blank_line_str);
        lcd.setCursor(0, 1);
        lcd.print(F("On "));

        lcd.print(Utilities::generateTimeString(config.pulse_width, true, false, true));

        lcd.setCursor(0, 1);

        lcd.blink();
    }

    void UI::selectOptionMenuItem2(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd){    

        lcd.noBlink();
        lcd.setCursor(0, 1);

        Display::LCDPrint_P(lcd, Display::blank_line_str);

        lcd.setCursor(0, 1);

        lcd.print(F("Off "));
        lcd.print(Utilities::generateTimeString(config.period-config.pulse_width, true, false, true));

        lcd.setCursor(0, 1);
        lcd.blink();
    }

    void UI::selecOptionMenuItem3(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd){

        lcd.noBlink();
        lcd.setCursor(0, 1);

        lcd.print(F("  Temperature  "));

        lcd.setCursor(2, 1);
        lcd.blink();
    }

    void UI::setPulseWidthMenu(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd){

        uint8_t edit_selection_index = 0;
        uint8_t scale = 60;
        
        lcd.noBlink();
        lcd.setCursor(0, 1);

        Display::LCDPrint_P(lcd, Display::blank_line_str);

        lcd.setCursor(0, 1);
        lcd.print("On ");

        String pw_pe_2 = "";

        pw_pe_2 += Utilities::generateTimeString(config.pulse_width, false, true, true);

        lcd.print(pw_pe_2);

        lcd.setCursor(9, 1);//on 00:00:00
        lcd.blink();

        uint32_t timer = millis();
        uint16_t pot_value = getscaledPotValue(scale);

        if(pot_value < 10)
            lcd.print('0');

        lcd.print(pot_value);

        uint8_t * tbreak = Utilities::timeBreakdown((uint64_t*)&config.pulse_width);

        while((uint32_t)((long)millis()-timer) < NUISANCE_PRESS_DURATION)
        {
            uint16_t new_pot_value = getscaledPotValue(scale);

            switch(edit_selection_index)
            {
                case 0: lcd.setCursor(9, 1);
                        break;

                case 1: lcd.setCursor(6, 1);
                        break;

                case 2: lcd.setCursor(3, 1);
                        break;
            }
            
            if(new_pot_value != pot_value)
            {
                timer = millis();
                pot_value = new_pot_value;

                if(pot_value < 10)
                    lcd.print('0');

                lcd.print(pot_value);
            }

            switch(edit_selection_index)
            {
                case 0: tbreak[3] = pot_value;
                break;

                case 1: tbreak[2] = pot_value;
                break;

                case 2: tbreak[1] = pot_value;
                break;
            }

            switch(getButtonPress())
            {
                case 0: if(++edit_selection_index == 3)
                            edit_selection_index = 0;

                        switch(edit_selection_index)
                        {
                            case 0: scale = 60;
                            break;

                            case 1: scale = 60;
                            break;

                            case 2: scale = 24;
                            break;
                        }

                        pot_value = 255;
                        timer = millis();

                        break;

                case 1: config.period -= config.pulse_width; 
                        config.pulse_width = (uint64_t)tbreak[3] + (uint64_t)tbreak[2] * SECONDS_IN_MINUTE + (uint64_t)tbreak[1] * SECONDS_IN_HOUR;
                        config.period += config.pulse_width;

                        if(config.pulse_width > config.period)
                            config.period = config.pulse_width;

                        if(config.period >= SECONDS_IN_DAY*2)
                            config.period = SECONDS_IN_DAY*2-1;

                        Persistance::setConfig(config);

                        return;
            }
        }
        return;    
    }

    void UI::setPeriodWidthMenu(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd){  //:)

        uint8_t edit_selection_index = 0;
        uint8_t scale = 60;

        uint64_t tdiff = config.period-config.pulse_width;
        
        lcd.noBlink();

        lcd.setCursor(0, 1);
        Display::LCDPrint_P(lcd, Display::blank_line_str);
        //lcd.print(F("                "));
        lcd.setCursor(0, 1);

        lcd.print(F("Off "));

        String pw_pe_2 = "";

        pw_pe_2 += Utilities::generateTimeString(tdiff, false, true, true);

        lcd.print(pw_pe_2);

        lcd.setCursor(10, 1);

        lcd.blink();

        uint32_t timer = millis();
        uint16_t pot_value = getscaledPotValue(scale);

        if(pot_value < 10)
        lcd.print('0');

        lcd.print(pot_value);

        uint8_t * tbreak = Utilities::timeBreakdown(&tdiff);

        while((uint32_t)((long)millis()-timer) < NUISANCE_PRESS_DURATION)
        {
            uint16_t new_pot_value = getscaledPotValue(scale);

            switch(edit_selection_index)
            {
                case 0: lcd.setCursor(10, 1);
                break;

                case 1: lcd.setCursor(7, 1);
                break;

                case 2: lcd.setCursor(4, 1);
                break;
            }
            
            if(new_pot_value != pot_value)
            {
                timer = millis();
                pot_value = new_pot_value;
                if(pot_value < 10)
                lcd.print('0');

                lcd.print(pot_value);
            }

            switch(edit_selection_index)
            {
                case 0: tbreak[3] = pot_value;
                        break;

                case 1: tbreak[2] = pot_value;
                        break;

                case 2: tbreak[1] = pot_value;
                        break;
            }

            switch(getButtonPress())
            {
                case 0: if(++edit_selection_index == 3)
                            edit_selection_index = 0;

                        switch(edit_selection_index)
                        {
                            case 0: scale = 60;
                            break;

                            case 1: scale = 60;
                            break;

                            case 2: scale = 24;
                            break;
                        }

                        pot_value = 255;
                        timer = millis();

                        break;

                case 1: 
                        config.period = (uint64_t)tbreak[3] + (uint64_t)tbreak[2] * SECONDS_IN_MINUTE + (uint64_t)tbreak[1] * SECONDS_IN_HOUR;
                        config.period += config.pulse_width;

                        if(config.period >= SECONDS_IN_DAY*2)
                            config.period = SECONDS_IN_DAY*2-1;

                        Persistance::setConfig(config);

                        return;
            }
        }

        return;
    }

    void UI::editMatchCondition(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd){

        uint8_t selected_option = config.cmp_options;

        lcd.noBlink();
        lcd.setCursor(0, 1);

        Display::LCDPrint_P(lcd, Display::match_less_set_str);

        lcd.setCursor(8, 1);

        if(selected_option)
            lcd.print(">");

        lcd.setCursor(8, 1);
        lcd.blink();

        uint32_t timer = millis();
        while((uint32_t)((long)millis()-timer) < NUISANCE_PRESS_DURATION)
        {
            switch(getButtonPress())
            {
                case 0: if(++selected_option == 2)
                            selected_option = 0;

                        switch(selected_option)
                        {
                            case 0: lcd.print('<');
                                    lcd.setCursor(8, 1);
                                    timer = millis();
                                    break;

                            case 1: lcd.print('>');
                                    lcd.setCursor(8, 1);
                                    timer = millis();
                                    break;
                        }

                        break;

                case 1: config.cmp_options = selected_option;
                        Persistance::setConfig(config);
                        return;
            }
        }
    }

    void UI::temperatureOptionMenuItem1(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd){

        lcd.noBlink();
        lcd.setCursor(0, 1);

        lcd.print(F("TC Enabled? "));

        if(config.enable_temperature_control)
            lcd.print("Yes   ");
        else
            lcd.print("No     "); 

        lcd.setCursor(0, 1);
        lcd.blink();
    }

    void UI::temperatureOptionMenuItem2(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd){

        lcd.noBlink();
        lcd.setCursor(0, 1);

        lcd.print(F("Setpoint        "));

        lcd.setCursor(9, 1);

        lcd.print(config.setpoint_0, 1);

        lcd.write((uint8_t)0b11011111);
        lcd.print('F');

        lcd.setCursor(0, 1);
        lcd.blink();
    }

    void UI::temperatureOptionMenuItem3(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd){

        lcd.noBlink();
        lcd.setCursor(0, 1);

        Display::LCDPrint_P(lcd, Display::match_less_set_str);

        lcd.setCursor(8, 1);

        if(config.cmp_options)
            lcd.print('>');

        lcd.setCursor(0, 1);
        lcd.blink();
    }

    void UI::temperatureOptionMenuItem4(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd){

        lcd.noBlink();
        lcd.setCursor(0, 1);

        lcd.print(F("Is Blocking?    "));

        lcd.setCursor(13, 1);

        if(config.tmp_ctl_is_blocking)
        lcd.print("Yes");
        else
        lcd.print("No ");

        lcd.setCursor(0, 1);
        lcd.blink();
    }

    void UI::temperatureOptionsMenu(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd){

        int menu_position = 0;
        temperatureOptionMenuItem1(config, lcd);

        uint32_t timer = millis();
        while((uint32_t)((long)millis()-timer) < NUISANCE_PRESS_DURATION)
        {
            switch(getButtonPress())
            {
                case 0: if(++menu_position == 4)
                            menu_position = 0;

                        timer = millis();

                        switch(menu_position)
                        {
                            case 0: temperatureOptionMenuItem1(config, lcd); //enable disable
                                    break;
                            case 1: temperatureOptionMenuItem2(config, lcd);
                                    break;
                            case 2: temperatureOptionMenuItem3(config, lcd);
                                    break;
                            case 3: temperatureOptionMenuItem4(config, lcd);
                                    break;
                        }

                        break;

                case 1: timer = millis();

                        switch(menu_position)
                        {
                            case 0: setTCEnable(config, lcd);
                                    break;
                            case 1: editSetpoint0(config, lcd);
                                    break;
                            case 2: editMatchCondition(config, lcd);
                                    break;
                            case 3: setBlockingEnable(config, lcd);
                                    break;

                        }
                    
                        return;
            }
        }
    }

    void UI::setTCEnable(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd)
    {
        uint8_t selected_option = config.enable_temperature_control ? 0 : 1;
        
        lcd.noBlink();
        lcd.setCursor(0, 1);

        Display::LCDPrint_P(lcd, Display::yes_no_str);

        switch(selected_option)
        {
            case 0: lcd.setCursor(2, 1);
                    break;

            case 1: lcd.setCursor(12, 1);
                    break;
        }

        lcd.blink();
        
        uint32_t timer = millis();
        while((uint32_t)((long)millis()-timer) < NUISANCE_PRESS_DURATION)
        {
            switch(selected_option)
            {
                case 0: lcd.setCursor(2, 1);
                        break;

                case 1: lcd.setCursor(12, 1);
                        break;
            }

            switch(getButtonPress())
            {
                case 0: if(++selected_option == 2)
                            selected_option = 0;

                        timer = millis();

                        break;
                        
                case 1: switch(selected_option)
                        {
                            case 0: config.enable_temperature_control = true;
                                    Persistance::setConfig(config);
                                    return;

                            case 1: config.enable_temperature_control = false;
                                    Persistance::setConfig(config);
                                    return;
                        }
            }
        }
    }

    void UI::setBlockingEnable(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd)
    {
        uint8_t selected_option = config.tmp_ctl_is_blocking;
        
        lcd.noBlink();
        lcd.setCursor(0, 1);

        Display::LCDPrint_P(lcd, Display::yes_no_str);

        switch(selected_option)
        {
            case 0: lcd.setCursor(12, 1);
                    break;

            case 1: lcd.setCursor(2, 1);
                    break;
        }

        lcd.blink();
        
        uint32_t timer = millis();
        while((uint32_t)((long)millis()-timer) < NUISANCE_PRESS_DURATION)
        {
            switch(selected_option)
            {
                case 0: lcd.setCursor(12, 1);
                        break;

                case 1: lcd.setCursor(2, 1);
                        break;
            }

            switch(getButtonPress())
            {
                case 0: if(++selected_option == 2)
                            selected_option = 0;
                        timer = millis();
                        break;

                case 1: switch(selected_option)
                        {
                            case 0: config.tmp_ctl_is_blocking = false;
                                    Persistance::setConfig(config);

                                    return;

                            case 1: config.tmp_ctl_is_blocking= true;
                                    Persistance::setConfig(config);

                                    return;
                        }
            }
        }
    }

    void UI::editSetpoint0(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd)
    {
        float val_bak = config.setpoint_0;
        float pot_value = 255;
        
        temperatureOptionMenuItem2(config, lcd);

        lcd.setCursor(9, 1);

        uint32_t timer = millis();
        while((uint32_t)((long)millis()-timer) < NUISANCE_PRESS_DURATION)
        {
            float new_pot_value  = (float)getscaledPotValue(1890)/10-4;

            if(new_pot_value != pot_value)
            {
                pot_value = new_pot_value;

                config.setpoint_0 = pot_value;

                lcd.print(pot_value, 1);
                lcd.write((uint8_t)0b11011111);
                lcd.print("F  ");

                lcd.setCursor(9, 1);

                timer = millis();
            }
            
            switch(getButtonPress())
            {
                case 0: timer = millis();
                        break;

                case 1: config.setpoint_0 = pot_value;
                        Persistance::setConfig(config);
                        return;
            }
        }

        config.setpoint_0 = val_bak;
    }

}