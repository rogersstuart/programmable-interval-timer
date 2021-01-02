#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <OneWire.h>
#include <DallasTemperature.h>
#include "Persistance.h"
#include <array>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define TEMP_INTEGRATION_DELAY 750

namespace PIT{

        class TemperatureSensing
        {
                private:

                        OneWire * one_wire;
                        DallasTemperature * sensors;
                        DeviceAddress * sensor_address;

                        TaskHandle_t * task_handle;
                        SemaphoreHandle_t * reading_lock;

                        std::array<float, 2> lrCoef;
                        uint8_t lrcoef_is_valid = false;

                        unsigned long lastTempRequest = 0; //last temperature request timestamp

                        uint64_t temp_times[60];
                        float temperatures[60]; //moving window
                        
                        int8_t t_head_index = 0;

                        bool t_ready = false;

                        void manageTemperatureSensor();
                        void sampleFill();
                        void getTemperatureTrend();

                public:

                        class SensorState{

                                public:

                                        bool is_ready = false;
                                        uint64_t capture_time;
                                        
                                        int head_index; //super lazy
                                        std::array<std::pair<uint64_t, float>*, 60> * samples = NULL;
                                        std::array<float, 2> * lrCoef = NULL;

                                        ~SensorState();

                                        float& getLatestTemperature();
                                        float getLinRegTemperature(float&& time);

                                private:

                                        friend class TemperatureSensing;

                                        SensorState(bool is_ready, uint64_t capture_time) : is_ready{is_ready}, capture_time{capture_time}{};
                        };

                        TemperatureSensing(::OneWire * oneWire);
                        ~TemperatureSensing();

                        void start();
                        void stop();

                        SensorState* getState();

                        static uint8_t tcheck(Persistance::PITConfig * config, SensorState * sensor_state);
        };

}

#endif //TEMPERATURE_SENSOR_H