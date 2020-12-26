#include <Arduino.h>
#include "Display.h"
#include "UI.h"

namespace PIT{

    void UI::menuSelection()
    {
        lcd.setCursor(0, 1);

        int button_press = getButtonPress();

        if(button_press == -1)
        {
            //display idle animation
            idleDisplay();
        }
        else
        if(button_press == 0)
        {
            
            if(run_mode == 2)
            LCDPrint_P(stop_pause_str);
                //lcd.print(F(" Stop     Pause "));
            else
                if(run_mode == 1)
                LCDPrint_P(stop_run_str);
                    //lcd.print(F(" Stop       Run "));
                else
                    if(run_mode == 0)
                    LCDPrint_P(run_str);
                        //lcd.print(F("       Run      "));
                        

            uint16_t pot_value = getscaledPotValue(2);

            switch(pot_value)
            {
                case 0: 
                
                switch(run_mode)
                {
                    case 2: lcd.setCursor(1 , 1); break;
                    case 1: lcd.setCursor(1 , 1);break;
                    case 0: lcd.setCursor(7 , 1);break;
                }
                break;
                case 1:
                switch(run_mode)
                {
                    case 2: lcd.setCursor(10 , 1); break;
                    case 1: lcd.setCursor(12 , 1);break;
                    case 0: lcd.setCursor(7 , 1);break;
                }
                break;
            }

            lcd.blink();
            
            uint32_t timer = millis();
            while((uint32_t)((long)millis()-timer) < NUISANCE_PRESS_DURATION)
            {
                pot_value = getscaledPotValue(2);

                switch(pot_value)
                {
                    case 0:
                    
                    switch(run_mode)
                    {
                        case 2: lcd.setCursor(1 , 1); break;
                        case 1: lcd.setCursor(1 , 1);break;
                        case 0: lcd.setCursor(7 , 1);break;
                    }
                    break;
                    case 1:
                    switch(run_mode)
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
                    case 0: //active_config.pulse_width = (uint64_t)pot_value;
                    //writeConfig();
                    //lcd.print(F("Short Press"));
                    //delay(1000);
                    return;
                    break;
                    case 1:
                    
                    if(run_mode == 2)
                    LCDPrint_P(stop_pause_str);
                    //lcd.print(F(" Stop     Pause "));
                    else
                    if(run_mode == 1)
                    LCDPrint_P(stop_run_str);
                    //lcd.print(F(" Stop       Run "));
                    else
                    if(run_mode == 0)
                    LCDPrint_P(run_str);
                    //lcd.print(F("       Run      "));
                    
                    switch(pot_value)
                    {
                        case 0:
                        
                        switch(run_mode)
                        {
                            case 2: setMode(0); break;
                            case 1: setMode(0);
                            case 0: setMode(2); break;
                        }
                        break;
                        case 1:
                        switch(run_mode)
                        {
                            case 2: setMode(1); break;
                            case 1: setMode(2); break;
                            case 0: setMode(2); break;
                        }
                        break;
                    }
                    return;
                    //delay(1000);
                    
                    break;
                }
            }
        }
        else
        if(button_press == 1)
        {
            selectOption();
        }    
    }

    static void UI::selectOption()
    {
        int menu_position = 0;

        switch(menu_position)
        {
            case 0: selectOptionMenuItem1();
            break;
            case 1: selectOptionMenuItem2();
            break;
            case 2: selecOptionMenuItem3();
            break;
        }

        uint32_t timer = millis();
        while((uint32_t)((long)millis()-timer) < NUISANCE_PRESS_DURATION)
        {
            switch(getButtonPress())
            {
                case 0:
                if(++menu_position == 3)
                    menu_position = 0;
                switch(menu_position)
                {
                    case 0: selectOptionMenuItem1();
                    break;
                    case 1: selectOptionMenuItem2();
                    break;
                    case 2: selecOptionMenuItem3();
                }
                break;
                case 1:

                switch(menu_position)
                {
                    case 0: setPulseWidthMenu();
                    break;
                    case 1: setPeriodWidthMenu();
                    break;
                    case 2: temperatureOptionsMenu();
                    break;

                }
                
                return;
                break;
            }
        }
    }

    static void UI::selectOptionMenuItem1()
    {
        lcd.noBlink();
        
        lcd.setCursor(0, 1);
        //lcd.print(F("                "));
        LCDPrint_P(blank_line_str);
        lcd.setCursor(0, 1);
        lcd.print(F("On "));

        lcd.print(Utilities.generateTimeString(active_config.pulse_width, true, false, true));

        lcd.setCursor(0, 1);

        lcd.blink();
    }

    static void UI::selectOptionMenuItem2()
    {    
        lcd.noBlink();
        
        lcd.setCursor(0, 1);
        //lcd.print(F("                "));
        LCDPrint_P(blank_line_str);
        lcd.setCursor(0, 1);
        lcd.print(F("Off "));

        lcd.print(Utilities.generateTimeString(active_config.period-active_config.pulse_width, true, false, true));

        lcd.setCursor(0, 1);

        lcd.blink();
    }

    static void UI::selecOptionMenuItem3()
    {
        lcd.noBlink();
        
        lcd.setCursor(0, 1);
        lcd.print(F("  Temperature  "));

        lcd.setCursor(2, 1);

        lcd.blink();
    }

    static void UI::setPulseWidthMenu()
    {
        uint8_t edit_selection_index = 0;
        uint8_t scale = 60;
        
        lcd.noBlink();

        lcd.setCursor(0, 1);
        //lcd.print(F("                "));
        LCDPrint_P(blank_line_str);
        lcd.setCursor(0, 1);

        lcd.print("On ");

        String pw_pe_2 = "";

        pw_pe_2 += Utilities.generateTimeString(active_config.pulse_width, false, true, true);

        lcd.print(pw_pe_2);

        lcd.setCursor(9, 1);//on 00:00:00

        lcd.blink();

        uint32_t timer = millis();
        uint16_t pot_value = getscaledPotValue(scale);

        if(pot_value < 10)
        lcd.print('0');

        lcd.print(pot_value);

        uint8_t * tbreak = timeBreakdown((uint64_t*)&active_config.pulse_width);

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
                case 1: active_config.period -= active_config.pulse_width; 
                        active_config.pulse_width = (uint64_t)tbreak[3] + (uint64_t)tbreak[2] * seconds_in_minute + (uint64_t)tbreak[1] * seconds_in_hour;
                        active_config.period += active_config.pulse_width;
                        if(active_config.pulse_width > active_config.period)
                            active_config.period = active_config.pulse_width;
                        if(active_config.period >= seconds_in_day*2)
                            active_config.period = seconds_in_day*2-1;
                        writeConfig();
                return;
                break;
            }
        }
        return;    
    }

    static void UI::setPeriodWidthMenu()  //:)
    {
        uint8_t edit_selection_index = 0;
        uint8_t scale = 60;

        uint64_t tdiff = active_config.period-active_config.pulse_width;
        
        lcd.noBlink();

        lcd.setCursor(0, 1);
        LCDPrint_P(blank_line_str);
        //lcd.print(F("                "));
        lcd.setCursor(0, 1);

        lcd.print(F("Off "));

        String pw_pe_2 = "";

        pw_pe_2 += Utilities.generateTimeString(tdiff, false, true, true);

        lcd.print(pw_pe_2);

        lcd.setCursor(10, 1);

        lcd.blink();

        uint32_t timer = millis();
        uint16_t pot_value = getscaledPotValue(scale);

        if(pot_value < 10)
        lcd.print('0');

        lcd.print(pot_value);

        uint8_t * tbreak = timeBreakdown(&tdiff);

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
                active_config.period = (uint64_t)tbreak[3] + (uint64_t)tbreak[2] * seconds_in_minute + (uint64_t)tbreak[1] * seconds_in_hour;
                active_config.period += active_config.pulse_width;
                if(active_config.period >= seconds_in_day*2)
                    active_config.period = seconds_in_day*2-1;
                writeConfig();
                return;
                break;
            }
        }
        return;
    }

    static void UI::temperatureOptionsMenu()
    {
        int menu_position = 0;
        temperatureOptionMenuItem1();

        uint32_t timer = millis();
        while((uint32_t)((long)millis()-timer) < NUISANCE_PRESS_DURATION)
        {
            switch(getButtonPress())
            {
                case 0:
                if(++menu_position == 4)
                menu_position = 0;
                timer = millis();
                switch(menu_position)
                {
                    case 0: temperatureOptionMenuItem1(); //enable disable
                    break;
                    case 1: temperatureOptionMenuItem2();
                    break;
                    case 2: temperatureOptionMenuItem3();
                    break;
                    case 3: temperatureOptionMenuItem4();
                    break;
                }
                break;
                case 1:
                timer = millis();
                switch(menu_position)
                {
                    case 0: setTCEnable();
                    break;
                    case 1: editSetpoint0();
                    break;
                    case 2: editMatchCondition();
                    break;
                    case 3: setBlockingEnable();
                    break;

                }
                
                return;
                break;
            }
        }
    }

    static void UI::editMatchCondition()
    {
        uint8_t selected_option = active_config.cmp_options;

        lcd.noBlink();

        lcd.setCursor(0, 1);

        LCDPrint_P(match_less_set_str);

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
                case 0:
                if(++selected_option == 2)
                selected_option = 0;
                switch(selected_option)
                {
                    case 0:
                    lcd.print('<');
                    lcd.setCursor(8, 1);
                    timer = millis();
                    break;
                    case 1:
                    lcd.print('>');
                    lcd.setCursor(8, 1);
                    timer = millis();
                    break;
                }
                break;
                case 1:
                active_config.cmp_options = selected_option;
                writeConfig();
                return;
                break;
            }
        }
    }

    static void UI::temperatureOptionMenuItem1()
    {
        lcd.noBlink();
        lcd.setCursor(0, 1);

        lcd.print(F("TC Enabled? "));

        if(active_config.enable_temperature_control)
            lcd.print("Yes   ");
        else
        lcd.print("No     "); 

        lcd.setCursor(0, 1);
        lcd.blink();
    }

    static void UI::temperatureOptionMenuItem2()
    {
        lcd.noBlink();
        lcd.setCursor(0, 1);

        lcd.print(F("Setpoint        "));

        lcd.setCursor(9, 1);

        lcd.print(active_config.setpoint_0, 1);

        lcd.write((uint8_t)0b11011111);
        lcd.print('F');

        lcd.setCursor(0, 1);
        lcd.blink();
    }

    static void UI::temperatureOptionMenuItem3()
    {
        lcd.noBlink();
        lcd.setCursor(0, 1);

        LCDPrint_P(match_less_set_str);

        lcd.setCursor(8, 1);

        if(active_config.cmp_options)
            lcd.print('>');

        lcd.setCursor(0, 1);
        lcd.blink();
    }

    static void UI::temperatureOptionMenuItem4()
    {
        lcd.noBlink();
        lcd.setCursor(0, 1);

        lcd.print(F("Is Blocking?    "));

        lcd.setCursor(13, 1);

        if(active_config.tmp_ctl_is_blocking)
        lcd.print("Yes");
        else
        lcd.print("No ");

        lcd.setCursor(0, 1);
        lcd.blink();
    }

    static void UI::setTCEnable()
    {
        uint8_t selected_option = active_config.enable_temperature_control ? 0 : 1;
        
        lcd.noBlink();
        
        lcd.setCursor(0, 1);

        LCDPrint_P(yes_no_str);

        switch(selected_option)
        {
            case 0:
            
            lcd.setCursor(2, 1);
            break;
            case 1:
            lcd.setCursor(12, 1);
            break;
        }

        lcd.blink();
        
        uint32_t timer = millis();
        while((uint32_t)((long)millis()-timer) < NUISANCE_PRESS_DURATION)
        {
            switch(selected_option)
            {
                case 0:
                
                lcd.setCursor(2, 1);
                break;
                case 1:
                lcd.setCursor(12, 1);
                
                break;
            }

            switch(getButtonPress())
            {
                case 0:
                if(++selected_option == 2)
                selected_option = 0;
                timer = millis();
                break;
                case 1:

                switch(selected_option)
                {
                    case 0: active_config.enable_temperature_control = true;
                            writeConfig();
                            return;
                    break;
                    case 1: active_config.enable_temperature_control = false;
                            writeConfig();
                            return;
                    break;

                }
                
                return;
                break;
            }
        }
    }

    static void UI::setBlockingEnable()
    {
        uint8_t selected_option = active_config.tmp_ctl_is_blocking;
        
        lcd.noBlink();
        
        lcd.setCursor(0, 1);

        LCDPrint_P(yes_no_str);

        switch(selected_option)
        {
            case 0:
            lcd.setCursor(12, 1);
            break;
            case 1:
            lcd.setCursor(2, 1);
            break;
        }

        lcd.blink();
        
        uint32_t timer = millis();
        while((uint32_t)((long)millis()-timer) < NUISANCE_PRESS_DURATION)
        {
            switch(selected_option)
            {
                case 0:
                lcd.setCursor(12, 1);
                break;
                case 1:
                lcd.setCursor(2, 1);
                break;
            }

            switch(getButtonPress())
            {
                case 0:
                if(++selected_option == 2)
                selected_option = 0;
                timer = millis();
                break;
                case 1:

                switch(selected_option)
                {
                    case 0: active_config.tmp_ctl_is_blocking = false;
                    writeConfig();
                    return;
                    break;
                    case 1: active_config.tmp_ctl_is_blocking= true;
                    writeConfig();
                    return;
                    break;

                }
                
                return;
                break;
            }
        }
    }

    static void UI::editSetpoint0()
    {
        float val_bak = active_config.setpoint_0;
        float pot_value = 255;
        
        temperatureOptionMenuItem2();

        lcd.setCursor(9, 1);

        uint32_t timer = millis();
        while((uint32_t)((long)millis()-timer) < NUISANCE_PRESS_DURATION)
        {
            float new_pot_value  = (float)getscaledPotValue(1890)/10-4;

            if(new_pot_value != pot_value)
            {
                pot_value = new_pot_value;

                active_config.setpoint_0 = pot_value;

                lcd.print(pot_value, 1);
                lcd.write((uint8_t)0b11011111);
                lcd.print("F  ");

                lcd.setCursor(9, 1);

                timer = millis();
            }
            
            switch(getButtonPress())
            {
                case 0:
                timer = millis();
                break;
                case 1:

                active_config.setpoint_0 = pot_value;
                writeConfig();
                
                return;
                break;
            }
        }

        active_config.setpoint_0 = val_bak;
    }

}