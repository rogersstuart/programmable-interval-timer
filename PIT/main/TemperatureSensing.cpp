#include "TemperatueSensor.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include "LinearRegression.h"

namespace PIT{

    TemperatueSensor::TemperatureSensor(OneWire * one_wire, _lock_t * one_wire_lock)
    {
        this->one_wire = one_wire;
        this->one_wire_lock = one_wire_lock;
        sensors = new DallasTemperature(one_wire);
    }

    void TemperatureSensor::stop()
    {
        xTaskDelete(task_handle);
    }

    //destructor
    TemperatureSensor::~TemperatureSensing(){
        stop();
        delete sensors;
    }

    void TemperatureSensor::start()
    {
        xTaskCreate([&](){
            while(true)
                manageTemperatureSensor();
        }], "t_sense_mgr", 8192, NULL, 1, &task_handle);
    }

    float TemperatueSensor::getLatestTemperature(){

        return temperatures[temperatures_index > 0 ? temperatures_index - 1 : 59];
    }

    float TemperatueSensor::getLinRegTemperature(float time){ //time in seconds
    
        uint64_t min_time = temp_times[temperatures_index];
        uint64_t max_time = temp_times[temperatures_index > 0 ? temperatures_index - 1 : 59];

        float span = (float)(max_time-min_time)/1000.0;

        float time_offset = (getSystemUptime() - max_time) / 1000.0;
            
        return (lrCoef[0]*(span+time_offset+time))+lrCoef[1];
    }

    /**
     * Fill the sample buffer.
     * 
     * @param none
     * @return none
     */
    void TemperatureSensor::sampleFill(){

        lcd.setCursor(0, 0);
        lcd.print(F("Sample Fill"));
        LCDPrint_P(blank_line_str);
        lcd.setCursor(0, 1);
        LCDPrint_P(blank_line_str);

        //prepare the sensor(s) for reading        
        sensors.getAddress(tempDeviceAddress, 0);
        sensors.setResolution(tempDeviceAddress, 12);
        sensors.setWaitForConversion(false);

        //collect samples and fill the buffer
        for(int iqw = 0; iqw < 60; iqw++)
        {
            if(!sensors.isConnected(tempDeviceAddress)) //if the process fails then break out an reattempt
                return;
            
            lcd.setCursor(0, 1);
            lcd.print(iqw+1);
            lcd.print(F(" of 60"));

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

    void TemperatueSensor::manageTemperatureSensor()
    {
        if(en_temp) //check to see if the temperature sensors have been initalized
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
        else{ //the temperature sensor needs to be initalized

            sensors.begin();

            if(sensors.getDeviceCount() > 0) //check to see if any sensors are connected
                sampleFill(); //if they are then fill the sample buffer
        }
    }

    uint8_t TemperatueSensor::compareTemperature(){

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

        for(uint8_t i = 1; i < 60; i++){

            if(temp_times[i] < min_time)
            min_time = temp_times[i];
        }

        float sample_times[60];

        for(int i = 0; i < 60; i++)
            sample_times[i] = (float)(temp_times[i] - min_time)/1000.0 ;
        
        LinearRegression::simpLinReg(sample_times, (float*)temperatures, (float*)lrCoef, 60);
    }

    

}
