#include "TemperatueSensor.h"

void TemperatureSensor::start()
{

}

void TemperatureSensor::stop()
{

}

void TemperatueSensor::TemperatureSensor(OneWire * one_wire, _lock_t * one_wire_lock)
{
    this->one_wire = one_wire;
    this->one_wire_lock = one_wire_lock;
    sensors = new DallasTemperature(one_wire);
}

void TemperatueSensor::manageTemperatureSensor()
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

uint8_t TemperatueSensor::compareTemperature()
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

void TemperatueSensor::getTemperatureTrend()
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

float TemperatueSensor::getLatestTemperature()
{
    return temperatures[temperatures_index > 0 ? temperatures_index - 1 : 59];
}

float TemperatueSensor::getLinRegTemperature(float time) //time in seconds
{
    uint64_t min_time = temp_times[temperatures_index];
    uint64_t max_time = temp_times[temperatures_index > 0 ? temperatures_index - 1 : 59];

    float span = (float)(max_time-min_time)/1000.0;

    float time_offset = (getSystemUptime() - max_time) / 1000.0;
        
    return (lrCoef[0]*(span+time_offset+time))+lrCoef[1];
}

