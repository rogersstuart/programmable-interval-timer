#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <array>

#define TEMP_INTEGRATION_DELAY 750

class OneWire;
class DallasTemperature;
class DeviceAddress;
class TaskHandle_t;
class SemaphoreHandle_t;

namespace PIT{

        namespace{

                //typedef unsigned char uint8_t;
                //typedef char int8_t;
                //typedef unsigned short uint16_t;
                //typedef unsigned long long uint64_t;
        }

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

                                private:

                                        SensorState() = default;

                                public:

                                        bool is_ready = false;
                                        uint64_t capture_time;
                                        
                                        int head_index; //super lazy
                                        std::array<std::pair<uint64_t, float>*, 60> * samples = NULL;
                                        std::array<float, 2> * lrCoef = NULL;

                                        ~SensorState();

                                        float& getLatestTemperature();
                                        float getLinRegTemperature(float&& time);
                        };

                        TemperatureSensing(OneWire * oneWire);
                        ~TemperatureSensing();

                        void start();
                        void stop();

                        SensorState* getState();

                        static uint8_t TemperatureSensing::tcheck(Persistance::PITConfig * config, TemperatureSensing::SensorState * sensor_state);
        };

}

#endif //TEMPERATURE_SENSOR_H