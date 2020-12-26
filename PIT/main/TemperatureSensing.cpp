#include "PIT.h"

#include <OneWire.h>
#include <DallasTemperature.h>

#include "TemperatureSensing.h"
#include "LinearRegression.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "Display/Display.h"
#include "Utilities.h"

#include "Persistance.h"

namespace PIT{

    /**
     * Constructor
     */
    TemperatureSensing::TemperatureSensing(OneWire * one_wire)
    {
        this->one_wire = one_wire;
        this->one_wire_lock = one_wire_lock;
        sensors = new DallasTemperature(one_wire);
    }

    void TemperatureSensing::stop()
    {
        vTaskDelete(task_handle);
        vSemaphoreDelete(reading_lock);
    }

    //destructor
    TemperatureSensing::~TemperatureSensing(){
        stop();
        delete sensors;
    }

    void TemperatureSensing::start()
    {
        xSemaphoreCreateBinary(reading_lock);
        xSemaphoreGive(reading_lock);
        
        xTaskCreate([&](){

            while(true)
            {
                xSemaphoreTake(reading_lock);
                manageTemperatureSensor();
                xSemaphoreGive(reading_lock);
                
                vTaskDelay(100); //give other tasks room
            }
                
        }, "t_sense_mgr", 8192, NULL, 1, &task_handle);
    }

    /**
     * Get the latest temperature.
     * 
     * @param none
     * @return the latest temperature
     */
    float TemperatureSensing::getLatestTemperature(){

        xSemaphoreTake(reading_lock, 0xFFFF);
        auto to_ret = temperatures[t_head_index > 0 ? t_head_index - 1 : 59];
        xSemaphoreGive(reading_lock);

        return to_ret;
    }

    /**
     * Get the estimated temperature at a point in time using a linear regression
     * 
     * @param time The time to calculate the temperature at.
     * @return The temperature at the specified time
     */
    float TemperatureSensing::getLinRegTemperature(float time){ //time in seconds
    
        xSemaphoreTake(reading_lock, 0xFFFF);
        
        uint64_t min_time = temp_times[t_head_index];
        uint64_t max_time = temp_times[t_head_index > 0 ? t_head_index - 1 : 59];

        float span = (float)(max_time-min_time)/1000.0;
        float time_offset = (Utilities::getSystemUptime() - max_time) / 1000.0;
        auto ret_val = (lrCoef[0]*(span+time_offset+time))+lrCoef[1];

        xSemaphoreGive(reading_lock);
            
        return ret_val;
    }

    /**
     * Fill the sample buffer.
     * 
     * @param none
     * @return none
     */
    void TemperatureSensing::sampleFill(){

        //it works against my organization but since "sample fill" has more to do with temperature sensing and less to do with UI
        //for now the UI related functions will stay here.

        //todo: Add a display task.
        
        auto display = Display::getInstance();
        auto lcd = display.checkOut();

        lcd.setCursor(0, 0);
        lcd.print(F("Sample Fill"));
        display.LCDPrint_P(Display::blank_line_str);
        lcd.setCursor(0, 1);
        display.LCDPrint_P(Display::blank_line_str);

        //prepare the sensor(s) for reading        
        sensors->getAddress(sensor_address, 0);
        sensors->setResolution(sensor_address, 12);
        sensors->setWaitForConversion(false);

        //collect samples and fill the buffer
        for(int iqw = 0; iqw < 60; iqw++){

            if(!sensors->isConnected(sensor_address)) //if the process fails then break out and reattempt
                return;
            
            lcd.setCursor(0, 1);
            lcd.print(iqw+1);
            lcd.print(F(" of 60"));

            sensors->requestTemperatures();

            delay(TEMP_INTEGRATION_DELAY);

            float temperature = sensors->getTempF(sensor_address);

            temperatures[iqw] = temperature;
            temp_times[iqw] = getSystemUptime();
        }

        sensors->requestTemperatures(); //prepare for the next reading
        lastTempRequest = millis();
        
        t_head_index = 0; //the buffer is full so set the head pointer to the beginning

        getTemperatureTrend();

        t_ready = true; //set flag to indicate that temperature sensing has been initalized successfully 
    }

    /**
     * Perform sensor management tasks.
     */
    void TemperatureSensing::manageTemperatureSensor(){

        if(t_ready){ //check to see if the temperature sensors have been initalized
        
            if(sensors->isConnected(sensor_address)){ //if the sensors have been initalized check to see if they're still connected

                if ((uint32_t)((long)millis() - lastTempRequest) >= temp_integration_delay){ //has the minimum reading interval passed?
                
                    lrcoef_is_valid = false; //adding a new sensor reading will invalidate the coefficients
                    
                    temperatures[t_head_index] = sensors.getTempF(sensor_address);
                    temp_times[t_head_index] = getSystemUptime();

                    sensors->requestTemperatures(); //prepare for the next round by beginning sensor integration
                    lastTempRequest = millis();

                    if(++t_head_index > 59) //move the head up or around
                        t_head_index = 0;

                    getTemperatureTrend();
                }
            }
            else{

                //there were no sensors disconnected or some other error occured
                t_ready = false;
                lrcoef_is_valid = false;
            }
        }
        else{ //the temperature sensor(s) needs to be initalized

            sensors->begin();

            if(sensors->getDeviceCount() > 0) //check to see if any sensors are connected
                sampleFill(); //if they are then fill the sample buffer
        }
    }

    /**
     * Do the stuff that needs to be done to calculate the linear regression coefficents.
     */
    void TemperatureSensing::getTemperatureTrend()
    {
        if(lrcoef_is_valid) //if there's no reason to update the coefficients then return
            return;
        
        lrCoef[0] = 0;
        lrCoef[1] = 0;

        //find the lowest time
        uint64_t min_time = temp_times[0];

        for(uint8_t i = 1; i < 60; i++)
            if(temp_times[i] < min_time)
                min_time = temp_times[i];

        //normalize the times
        float sample_times[60];
        for(int i = 0; i < 60; i++)
            sample_times[i] = (float)(temp_times[i] - min_time)*0.001 ;
        
        //perform the regression calculation
        LinearRegression::simpLinReg(sample_times, (float*)temperatures, (float*)lrCoef, 60);

        lrcoef_is_valid = true;
    }

    bool TemperatureSensing::isReady(){
        return t_ready;
    }
}
