#include "PIT.h"

const uint64_t seconds_in_day = 86400;
const uint64_t seconds_in_hour = 3600;
const uint64_t seconds_in_minute = 60; 

uint16_t pot_running_avg_elements[NUM_POT_AVG_ELEMENTS];
uint8_t pot_running_avg_elements_index = 0;

volatile uint16_t current_pot_value;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

OneWire oneWire(10);

////
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;

uint8_t lrcoef_is_valid = false;

unsigned long lastTempRequest = 0;
const uint16_t temp_integration_delay = 750;

uint64_t temp_times[60];

float temperatures[60];
int8_t temperatures_index = 0;

float lrCoef[2] = {0, 0};

////

uint32_t press_detection_time = 0;
uint8_t button_press_detected = false;

uint64_t system_uptime = 0;

uint8_t run_mode = 0;

uint64_t uptime_at_cycle_start = 0;
uint64_t uptime_at_pause = 0;

uint32_t button_block_timer = 0;

uint8_t en_temp = false;

uint8_t force_extern = 0;

const char stop_pause_str[] PROGMEM = " Stop     Pause ";
const char stop_run_str[] PROGMEM = " Stop       Run ";
const char run_str[] PROGMEM = "       Run      ";
const char yes_no_str[] PROGMEM = "  Yes       No  ";
const char match_less_set_str[] PROGMEM = "Match t < set   ";

const char blank_line_str[] PROGMEM = "                ";

struct config
{
    uint64_t pulse_width = 60;
    uint64_t period = 120;
    uint8_t run_on_power_up = true;
    uint8_t enable_temperature_control = false;
    float setpoint_0 = 78.0f;
    uint8_t cmp_options = 0; //0 is less than, 1 is greater than
    uint8_t tmp_ctl_is_blocking = false;
} active_config;

void app_main()
{
    initArduino();
}

void setup()
{
    Serial.begin(250000);
    
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    pinMode(A0, INPUT_PULLUP);
    pinMode(A2, INPUT_PULLUP);
    pinMode(A3, INPUT_PULLUP);
    
    lcd.begin (16,2);
    lcd.backlight();
    lcd.home ();// go home

    lcd.print(F("PIT 1.1.20"));
    lcd.setCursor(0, 1);
    lcd.print(F("MST LLC 2016"));

    delay(4000);

    lcd.createChar(0, playChar);
    lcd.createChar(1, pauseChar);
    lcd.createChar(2, stopChar);
    lcd.createChar(3, playTempEn);
    lcd.createChar(4, runTmpTmr_norm);
    lcd.createChar(5, tlvl);
    lcd.createChar(6, tup);
    lcd.createChar(7, tdown);

    readConfig();

    attachInterrupt(digitalPinToInterrupt(2), buttonPressDetection, FALLING);

    Timer1.initialize(1000000/60/32);
    
    delay(10);
    
    button_press_detected = false;

    current_pot_value = analogRead(POT_PIN);

    if(active_config.run_on_power_up)
        setMode(2);
    else
        setMode(0);
    
    MsTimer2::set(10, processTimer);
    MsTimer2::start();
}

void loop()
{
    manageTemperatureSensor();

    displayStatusLine();
    
    Timer1.attachInterrupt(updatePotReading);
    menuSelection();
    Timer1.detachInterrupt();

    if(Serial.available() >= 2)
        if(Serial.read() == 0b10101111)
            processSerial();
}

void processSerial()
{
    uint8_t cfg_byte = Serial.read();
    Serial.write(0b11111010);

    if(cfg_byte & 1)
        force_extern = cfg_byte & 3;
    else
        force_extern = 0;
}

void manageTemperatureSensor()
{
    if(en_temp)
    {
        if(sensors.isConnected(tempDeviceAddress))
        {
            if ((uint32_t)((long)millis() - lastTempRequest) >= temp_integration_delay) // waited long enough??
            {
                lrcoef_is_valid = false;
                
                temperatures[temperatures_index] = sensors.getTempFByIndex(0);
                temp_times[temperatures_index] = getSystemUptime();

                sensors.requestTemperatures();
                lastTempRequest = millis();

                if(++temperatures_index > 59)
                temperatures_index = 0;

                getTemperatureTrend();

                lrcoef_is_valid = true;
            }
        }
        else
        {
            en_temp = false;
            lrcoef_is_valid = false;
        }
    }
    else
    {
        sensors.begin();

        if(sensors.getDeviceCount() > 0)
        {
            lcd.setCursor(0, 0);
            lcd.print(F("Sample Fill"));
            LCDPrint_P(blank_line_str);
            lcd.setCursor(0, 1);
            LCDPrint_P(blank_line_str);
            
            
            sensors.getAddress(tempDeviceAddress, 0);
            sensors.setResolution(tempDeviceAddress, 12);

            sensors.setWaitForConversion(false);

            for(int iqw = 0; iqw < 60; iqw++)
            {
                if(!sensors.isConnected(tempDeviceAddress))
                    return;
                
                lcd.setCursor(0, 1);
                lcd.print(iqw+1);
                lcd.print(F(" of 60"));
                //LCDPrint_P(blank_line_str);

                sensors.requestTemperatures();

                delay(temp_integration_delay);

                float temperature = sensors.getTempFByIndex(0);

                temperatures[iqw] = temperature;
                temp_times[iqw] = getSystemUptime();
            }

            sensors.requestTemperatures();
            lastTempRequest = millis();
            
            temperatures_index = 0;

            getTemperatureTrend();
            lrcoef_is_valid = true;
            en_temp = true;

            button_press_detected = false;          
        }
    }
}

void LCDPrint_P(const char str[])
{
   char c;
   if(!str)
    return;

   while((c = pgm_read_byte(str++)))
    lcd.print(c);
}

void displayStatusLine()
{
    lcd.noBlink();
    
    lcd.setCursor(0, 0);
     
    uint64_t current_time = getSystemUptime();
    uint64_t cycle_difference = (active_config.period - ((current_time - uptime_at_cycle_start) / 1000.0));
 
    String pw_pe = "";

    if(run_mode == 2)
    {
        if(!(active_config.enable_temperature_control && active_config.tmp_ctl_is_blocking && en_temp))
            pw_pe += generateTimeString(cycle_difference, true, false, false);
    }
    else
        if(run_mode == 0)
        {
            if(!(active_config.enable_temperature_control && active_config.tmp_ctl_is_blocking && en_temp))
                pw_pe += String("0");
        }
        else
            if(run_mode == 1)
            {
                cycle_difference = (active_config.period - ((uptime_at_pause - uptime_at_cycle_start) / 1000.0));

                pw_pe += generateTimeString(cycle_difference, true, false, false);                
            }

    if(en_temp)
    {
        if(!(active_config.enable_temperature_control && active_config.tmp_ctl_is_blocking && en_temp))
        {
            pw_pe += " " + String(getLinRegTemperature(0), 1);
        }
        else
        {
            pw_pe += String(getLinRegTemperature(0), 2);

            pw_pe += String((char)0b11011111) + "F";
        }

        
        if(active_config.enable_temperature_control && active_config.tmp_ctl_is_blocking && en_temp)
        {
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

        if(active_config.enable_temperature_control && en_temp && active_config.tmp_ctl_is_blocking)
            lcd.write((uint8_t)3);
        else
            if(active_config.enable_temperature_control && en_temp)
                lcd.write((uint8_t)4);
            else
                lcd.write((uint8_t)0);
        break;
    } 
       
}
/*
float getFullTemperatureAvg()
{
      float average = 0;
      for(int i = 0; i < 60; i++)
      average += temperatures[i];
      average /= 60;
      return average;
}

float get10sTemperatureAvg()
{
    float average = 0;

    int first_sample = temperatures_index > 0 ? temperatures_index - 1 : 59;
    uint64_t tat0 = temp_times[first_sample];

    int samples = 0;
    for(int i = first_sample; tat0 - temp_times[i] <= 10000;)
    {
        samples++;
        average += temperatures[i];
        
        if(--i < 0)
            i = 59;       
    }

    return average / (float)samples;
}
*/
float getLatestTemperature()
{
    return temperatures[temperatures_index > 0 ? temperatures_index - 1 : 59];
}

float getLinRegTemperature(float time) //time in seconds
{
    uint64_t min_time = temp_times[temperatures_index];
    uint64_t max_time = temp_times[temperatures_index > 0 ? temperatures_index - 1 : 59];

    float span = (float)(max_time-min_time)/1000.0;

    float time_offset = (getSystemUptime() - max_time) / 1000.0;
        
    return (lrCoef[0]*(span+time_offset+time))+lrCoef[1];
}

void getTemperatureTrend()
{
    if(lrcoef_is_valid)
        return;
    
    //float temp_cpy[60];
    //memcpy((uint8_t*)&temp_cpy, (uint8_t*)&temperatures, sizeof(float)*60);
    
    lrCoef[0] = 0;
    lrCoef[1] = 0;

    uint64_t min_time = temp_times[0];
    //uint8_t tcheck = 0;

    for(uint8_t i = 1; i < 60; i++)
    {
        if(temp_times[i] < min_time)
        min_time = temp_times[i];

       //if(temperatures[0] == temperatures[i])
       //tcheck++;
    }

   // if(tcheck < 29)
   // {
    float sample_times[60];

    for(int i = 0; i < 60; i++)
        sample_times[i] = (float)(temp_times[i] - min_time)/1000.0 ;
    //    sample_times[i] = ((float)i *(750.0 / (1 << (12 - resolution))))/1000;
        //sample_times[start_index+i > 59 ? 59-(start_index+i) : start_index+i] = ((float)i *(750.0 / (1 << (12 - resolution))))/1000;

    
       
    
    simpLinReg(sample_times, (float*)temperatures, (float*)lrCoef, 60);
 //   }
  //  else
  //  {
        // lrCoef[0] = 0;
        // lrCoef[1] = getLatestTemperature();
   // }
}

float xbar;
float ybar;
float xybar;
float xsqbar;

void simpLinReg(float* x, float* y, float* lrCoef, uint8_t num){
    // pass x and y arrays (pointers), lrCoef pointer, and n.  The lrCoef array is comprised of the slope=lrCoef[0] and intercept=lrCoef[1].  n is length of the x and y arrays.

    // initialize variables
    xbar=0.0;
    ybar=0.0;
    xybar=0.0;
    xsqbar=0.0;
    
    // calculations required for linear regression
    for (int i=0; i<num; i++)
    {
        xbar=xbar+x[i];
        ybar=ybar+y[i];
        xybar=xybar+x[i]*y[i];
        xsqbar=xsqbar+x[i]*x[i];
    }
    xbar/=num;
    ybar/=num;
    xybar/=num;
    xsqbar/=num;
    
    // simple linear regression algorithm
    lrCoef[0]=(xybar-xbar*ybar)/(xsqbar-xbar*xbar);
    lrCoef[1]= (ybar-lrCoef[0]*xbar);
}

uint64_t getSystemUptime()
{
    system_uptime += (uint64_t)((long)millis()-(uint32_t)system_uptime);
    return system_uptime;
}

int8_t getButtonPress()
{
    int8_t ret_val = -1;

    if(button_press_detected)
    {
        uint32_t press_detection_time_cpy = press_detection_time;
        
        delay(BUTTON_READ_BLOCK_DURATION);

        while(!digitalRead(BUTTON_PIN))
            if((uint32_t)((long)millis()-press_detection_time_cpy) >= LONG_PRESS_DURATION)
                break;

        if((uint32_t)((long)millis()-press_detection_time_cpy) >= LONG_PRESS_DURATION)
            ret_val = 1;
        else
            if((uint32_t)((long)millis()-press_detection_time_cpy) >= SHORT_PRESS_DURATION)
                ret_val = 0;

        delay(BUTTON_READ_BLOCK_DURATION);

        button_press_detected = false;
    }

    return ret_val;
}

uint8_t t_chk_res = false;
void processTimer()
{
    if(run_mode == 2)
    {
        t_chk_res = tcheck();
        
        uint64_t current_time = getSystemUptime();
        float cycle_difference = (current_time - uptime_at_cycle_start) / 1000.0;

        if(cycle_difference >= active_config.period)
        {
            uptime_at_cycle_start = current_time;
            cycle_difference = 0;
        }

        if(force_extern == 0)
        {
            if(!(active_config.enable_temperature_control && active_config.tmp_ctl_is_blocking && en_temp))
            {
                if((uint32_t)cycle_difference >= active_config.pulse_width && !t_chk_res)
                    pinMode(4, INPUT); //off
                else
                    pinMode(4, OUTPUT); //on
            }
            else
            {
                if(t_chk_res)
                     pinMode(4, OUTPUT); //on 
                else
                    pinMode(4, INPUT); //off    
            } 
        }
        else
        {
            if(force_extern >> 1)
                 pinMode(4, OUTPUT); //on
            else
                pinMode(4, INPUT); //off
        }  
    }
    else
        if(run_mode == 0)
            pinMode(4, INPUT);    
}

uint8_t tcheck()
{
    if(en_temp && active_config.enable_temperature_control)
    {
        if(!lrcoef_is_valid)
            return t_chk_res; //just skip this round

        //float temperature = lrt;
        if(active_config.cmp_options == 0) //less than
        {
          if(getLinRegTemperature(0) < active_config.setpoint_0)
            return true;
          else
            return false; 
        }
        else
            if(active_config.cmp_options == 1)
            {
                if(getLinRegTemperature(0) > active_config.setpoint_0)
                return true;
                else
                return false;
            }
            else
                return false;
    }
    else
        return false;
}

void idleDisplay()
{
    lcd.setCursor(0, 1);
    
    if(active_config.enable_temperature_control && active_config.tmp_ctl_is_blocking && en_temp)
    {
        if(!lrcoef_is_valid)
            return;
        
        //lcd.print(F("                "));
        lcd.setCursor(0, 1);
        
        float range = active_config.setpoint_0 - getLinRegTemperature(0);

        //int64_t time_when_match = round((active_config.setpoint_0 - lrCoef[1]) / lrCoef[0]);

        //Serial.println(time_when_match);

        //lcd.print(String(time_when_match, 2));
        //if(time_when_match < seconds_in_hour && time_when_match >= 0)
        //    lcd.print(generateTimeString(time_when_match, false, false, false));
        //else
        //    lcd.print("WNI");
        lcd.print(range, 2);
        lcd.write((uint8_t)0b11011111);
        lcd.print("F ");

        if(tcheck())
            lcd.print("TRIG");
        else
            lcd.print("NORM");

        LCDPrint_P(blank_line_str);

        /*
        range *= 1.5;
        
        if(range < 0)
            range *= -1;
        if(range < 2)
            range = 2;

        float block_val = range/16.0;

        if(current_temperature < active_config.setpoint_0)
        {
            uint8_t min_blk = round(((active_config.setpoint_0 - current_temperature)/range)*block_val);
            lcd.setCursor(min_blk, 1);
            for(uint8_t i = min_blk; i < 16; i++)
                lcd.print((char)255);           
        }
        else
            if(current_temperature > active_config.setpoint_0)
            {
                uint8_t min_blk = round(((current_temperature - active_config.setpoint_0)/range)*block_val);
                lcd.setCursor(0, 1);
                for(uint8_t i = 0; i <= min_blk; i++)
                    lcd.print((char)255);
            }  
            */
                      
    }
    else
    {
        uint64_t current_uptime = getSystemUptime();

        int num_blocks = round(((run_mode == 1 ? uptime_at_pause : current_uptime - uptime_at_cycle_start) / 1000.0 / active_config.period) * 16);
        int pulse_blocks = round(((float)active_config.pulse_width/active_config.period)*16);

        for(int i = 0; i < 16; i++)
            if(i < num_blocks)
            {
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


void setMode(uint8_t new_mode)
{
    if(run_mode == new_mode)
        return;

    if((run_mode == 0 && new_mode == 2) || (run_mode == 0 && new_mode == 1))
    {
        run_mode = new_mode;
        uptime_at_cycle_start = getSystemUptime();
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
        uint64_t new_uptime = getSystemUptime();
        uint64_t offset = new_uptime - uptime_at_pause;
        uptime_at_cycle_start += offset;
        return;
    }

    if(run_mode == 2 && new_mode == 1)
    {
        run_mode = new_mode;
        uptime_at_pause = getSystemUptime();
        return;  
    }
}
 

uint16_t getscaledPotValue(uint16_t max)
{
    return (uint16_t)(((float)current_pot_value/MAX_POT_VALUE)*max);
}

void updatePotReading()
{
    pot_running_avg_elements[pot_running_avg_elements_index] = analogRead(POT_PIN);
    if(++pot_running_avg_elements_index == NUM_POT_AVG_ELEMENTS)
    {
        uint64_t sum = 0;
        for(uint8_t i = 0; i < NUM_POT_AVG_ELEMENTS; i++)
            sum += pot_running_avg_elements[i];
        current_pot_value = sum/NUM_POT_AVG_ELEMENTS;
        pot_running_avg_elements_index = 0;
    }
}

void buttonPressDetection()
{
    button_block_timer = millis();
    press_detection_time = millis();
    button_press_detected = true;
}


void readConfig()
{
    uint16_t bringup_match = (uint16_t)EEPROM.read(0) << 8 | EEPROM.read(1);
    char * bp = (char*)&active_config;
    if(bringup_match == BRINGUP_CODE)
    {
        for(uint8_t i = 0; i < sizeof(config); i++)
            bp[i] = EEPROM.read(i+2);
    }
    else
        writeConfig();  
}

void writeConfig()
{
    char * bp = (char*)&active_config;
    for(uint8_t i = 0; i < sizeof(config); i++)   
        EEPROM.write(i+2, (uint8_t)bp[i]);

    uint16_t bringup_match = (uint16_t)EEPROM.read(0) << 8 | EEPROM.read(1);
    if(bringup_match != BRINGUP_CODE)
    {
        EEPROM.write(0, BRINGUP_CODE >> 8);
        EEPROM.write(1, BRINGUP_CODE & 0xFF);
    }   
}
 
void menuSelection()
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


void selectOption()
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

uint8_t* timeBreakdown(uint64_t * seconds)
{
    static uint8_t tbreak[4];
    
    tbreak[0] = *seconds/seconds_in_day;
    tbreak[1] = (*seconds%seconds_in_day)/seconds_in_hour;
    tbreak[2] = ((*seconds%seconds_in_day)%seconds_in_hour)/seconds_in_minute;
    tbreak[3] = ((*seconds%seconds_in_day)%seconds_in_hour)%seconds_in_minute;

    return (uint8_t*)&tbreak;
}


String generateTimeString(uint64_t total_seconds, uint8_t en_days, uint8_t zero_pad, uint8_t force_full_display)
{
    uint8_t * tbreak = timeBreakdown(&total_seconds);

    String str = "";

    if(tbreak[0] > 0 && en_days)
        str += String(tbreak[0]) + ":";

    if(tbreak[1] > 0 || (tbreak[0] > 0 && en_days) || force_full_display)
    {
        if((zero_pad || force_full_display) && tbreak[1] < 10)
            str += "0";

        str +=  String(tbreak[1]) + ":";
    }

    if(tbreak[2] > 0 || (tbreak[1] > 0 || (tbreak[0] > 0 && en_days)) || force_full_display)
    {
        if((zero_pad || force_full_display) && tbreak[2] < 10)
            str += "0";

        str +=  String(tbreak[2]) + ":";
    }

    if((zero_pad || force_full_display) && tbreak[3] < 10)
        str += "0";
    
    str +=  String(tbreak[3]);

    return str;   
}


void selectOptionMenuItem1()
{
    lcd.noBlink();
    
    lcd.setCursor(0, 1);
    //lcd.print(F("                "));
    LCDPrint_P(blank_line_str);
    lcd.setCursor(0, 1);
    lcd.print(F("On "));

    lcd.print(generateTimeString(active_config.pulse_width, true, false, true));

    lcd.setCursor(0, 1);

    lcd.blink();
}

void selectOptionMenuItem2()
{    
    lcd.noBlink();
    
    lcd.setCursor(0, 1);
    //lcd.print(F("                "));
    LCDPrint_P(blank_line_str);
    lcd.setCursor(0, 1);
    lcd.print(F("Off "));

    lcd.print(generateTimeString(active_config.period-active_config.pulse_width, true, false, true));

    lcd.setCursor(0, 1);

    lcd.blink();
}

void selecOptionMenuItem3()
{
    lcd.noBlink();
    
    lcd.setCursor(0, 1);
    lcd.print(F("  Temperature  "));

    lcd.setCursor(2, 1);

    lcd.blink();
}

void setPulseWidthMenu()
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

    pw_pe_2 += generateTimeString(active_config.pulse_width, false, true, true);

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

void setPeriodWidthMenu()  //:)
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

    pw_pe_2 += generateTimeString(tdiff, false, true, true);

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

void temperatureOptionsMenu()
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

void editMatchCondition()
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

void temperatureOptionMenuItem1()
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

void temperatureOptionMenuItem2()
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

void temperatureOptionMenuItem3()
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

void temperatureOptionMenuItem4()
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

void setTCEnable()
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

void setBlockingEnable()
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

void editSetpoint0()
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

