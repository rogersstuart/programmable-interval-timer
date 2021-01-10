#include "PIT.h"
#include "PIT.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#include "LinearRegression.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Display/Display.h"
#include "Utilities.h"
#include "Persistance.h"
#include <vector>
#include <map>
#include "TemperatureSensing.h"
#include <LiquidCrystal_I2C.h>

#define ENABLE_SAMPLE_FILL_MESSAGE true

namespace PIT{

    /**
     * Constructor
     */
    TemperatureSensing::TemperatureSensing(OneWire * one_wire)
    {
        this->one_wire = one_wire;
        sensors = DallasTemperature(one_wire);
    }

    void TemperatureSensing::stop()
    {
        vTaskDelete(task_handle);
        vSemaphoreDelete(reading_lock);
    }

    //destructor
    TemperatureSensing::~TemperatureSensing(){
        stop();
        //delete sensors;
    }

    void TemperatureSensing::start()
    {
        reading_lock = xSemaphoreCreateMutex();
        xSemaphoreGive(reading_lock);
        
        xTaskCreatePinnedToCore([](void * self){

            while(true)
            {
                auto capture = (TemperatureSensing*)self;
                xSemaphoreTake(capture->reading_lock, 0xFFFF);
                
                if(Persistance::getConfig().enable_temperature_control)
                    capture->manageTemperatureSensor();
                else
                {
                    capture->t_ready = false;
                    capture->lrcoef_is_valid = false;
                }

                xSemaphoreGive(capture->reading_lock);
                
                vTaskDelay(100); //give other tasks room
            }
                
        }, "t_sense_mgr", 8192, this, 2, &task_handle, 0);
    }

    /**
     * Get the latest temperature.
     * 
     * @param none
     * @return the latest temperature
     */
    float& TemperatureSensing::SensorState::getLatestTemperature(){

        return (*samples)[head_index > 0 ? head_index - 1 : 59].second;
    }

    /**
     * Get the estimated temperature at a point in time using a linear regression
     * 
     * @param time The time to calculate the temperature at.
     * @return The temperature at the specified time
     */
    float TemperatureSensing::SensorState::getLinRegTemperature(float&& time){ //time in seconds
    
        uint64_t& min_time = (*samples)[head_index].first;
        uint64_t& max_time = (*samples)[head_index > 0 ? head_index - 1 : 59].first;

        float span = (max_time-min_time) * 0.001f;
        float time_offset = (Utilities::getSystemUptime() - max_time) * 0.001f;
        auto ret_val = ((*lrCoef)[0] * (span + time_offset + time)) + (*lrCoef)[1];

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

        #ifdef ENABLE_SAMPLE_FILL_MESSAGE
        auto display = Display::getInstance();
        auto lcd = display.checkOut();

        lcd.clear();
        lcd.setCursor(0, 0);

        lcd.print(F("**Sample Fill**"));
        Display::LCDPrint_P(lcd, Display::blank_line_str);
        lcd.setCursor(0, 1);
        Display::LCDPrint_P(lcd, Display::blank_line_str);
        lcd.setCursor(0, 1);
        lcd.print(F("Preparing..."));
        #endif

        //prepare the sensor(s) for reading        
        sensors.getAddress(sensor_address, 0);
        sensors.setResolution(sensor_address, 12);
        sensors.setWaitForConversion(false);

        //collect samples and fill the buffer
        for(int iqw = 0; iqw < 60; iqw++){

            if(!sensors.isConnected(sensor_address)) //if the process fails then break out and reattempt
                return;

            sensors.requestTemperatures();

            delay(TEMP_INTEGRATION_DELAY);

            float temperature = sensors.getTempF(sensor_address);

            #ifdef ENABLE_SAMPLE_FILL_MESSAGE
            lcd.setCursor(0, 1);
            lcd.print(iqw+1);
            lcd.print(F("/60"));

            lcd.print(" -> ");
            lcd.print(temperature);
            lcd.print("F");
            #endif

            temperatures[iqw] = temperature;
            temp_times[iqw] = Utilities::getSystemUptime();
        }

        sensors.requestTemperatures(); //prepare for the next reading
        lastTempRequest = millis();
        
        t_head_index = 0; //the buffer is full so set the head pointer to the beginning

        getTemperatureTrend();

        t_ready = true; //set flag to indicate that temperature sensing has been initalized successfully 

        #ifdef ENABLE_SAMPLE_FILL_MESSAGE
        display.checkIn(lcd);
        #endif
    }

    /**
     * Perform sensor management tasks.
     */
    void TemperatureSensing::manageTemperatureSensor(){

        if(t_ready){ //check to see if the temperature sensors have been initalized

        //sensors.
        
            if(sensors.isConnected(sensor_address)){ //if the sensors have been initalized check to see if they're still connected

                if ((uint32_t)((long)millis() - lastTempRequest) >= TEMP_INTEGRATION_DELAY){ //has the minimum reading interval passed?
                
                    lrcoef_is_valid = false; //adding a new sensor reading will invalidate the coefficients
                    
                    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
                    portENTER_CRITICAL(&mux);
                    
                    temperatures[t_head_index] = sensors.getTempF(sensor_address);
                    temp_times[t_head_index] = Utilities::getSystemUptime();
                    
                    sensors.requestTemperatures(); //prepare for the next round by beginning sensor integration
                    portEXIT_CRITICAL(&mux);

                    lastTempRequest = millis();

                    if(++t_head_index > 59) //move the head up or around
                        t_head_index = 0;

                    getTemperatureTrend();
                }
            }
            else{

                //there were no sensors disconnected or some other error occured
                //t_ready = false;
                //lrcoef_is_valid = false;
            }
        }
        else{ //the temperature sensor(s) needs to be initalized

            sensors.begin();

            if(sensors.getDeviceCount() > 0) //check to see if any sensors are connected
                sampleFill(); //if they are then fill the sample buffer
        }
    }

    /**
     * Do the things that need to be done to calculate the linear regression coefficents.
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
        float norm_sample_times[60];
        for(int i = 0; i < 60; i++)
            norm_sample_times[i] = (temp_times[i] - min_time)*0.001F ;
        
        //perform the regression calculation
        LinearRegression::linearRegression(norm_sample_times, (float*)temperatures, lrCoef, 60);

        lrcoef_is_valid = true;
    }

    TemperatureSensing::SensorState::~SensorState(){
        delete lrCoef;
        delete samples;
    }

    TemperatureSensing::SensorState* TemperatureSensing::getState(){
  
        xSemaphoreTake(reading_lock, 0xFFFF);

        auto state = new SensorState{t_ready, Utilities::getSystemUptime()};

        if(t_ready)
        {
            state->head_index = t_head_index;
            
            state->lrCoef = new std::vector<float>(2);
            *(state->lrCoef) = lrCoef;

            state->samples = new std::vector<std::pair<uint64_t, float>>(60);

            for(int i = 0; i < state->samples->size(); i++){
                (*(state->samples))[i] = std::pair<uint64_t, float>(temp_times[i], temperatures[i]);
            }
        }

        xSemaphoreGive(reading_lock);

        return state;
    }

    //idk where else to put it
    uint8_t TemperatureSensing::tcheck(Persistance::PITConfig * config, TemperatureSensing::SensorState * sensor_state)
    {
        if(sensor_state->is_ready && config->enable_temperature_control)
        {
            //if(!lrcoef_is_valid)
            //    return t_chk_res; //just skip this round

            if(config->cmp_options == 0) //less than
            {
                if(sensor_state->getLinRegTemperature(0) < config->setpoint_0)
                    return true;
                else
                    return false; 
            }
            else
                if(config->cmp_options == 1)
                {
                    if(sensor_state->getLinRegTemperature(0) > config->setpoint_0)
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
}
