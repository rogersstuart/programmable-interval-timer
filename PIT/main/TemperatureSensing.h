#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#define TEMP_INTEGRATION_DELAY 750

namespace PIT{

        namespace{

                extern class OneWire;
                extern class DallasTemperature;
                extern class DeviceAddress;
                extern class TaskHandle_t;
                extern class SemaphoreHandle_t;

                typedef unsigned char uint8_t;
                typedef char int8_t;
                typedef unsigned short uint16_t;
                typedef unsigned long long uint64_t;
        }

        class TemperatureSensing
        {
                private:

                        OneWire * one_wire;
                        DallasTemperature * sensors;
                        DeviceAddress * sensor_address;

                        TaskHandle_t * task_handle;
                        SemaphoreHandle_t * reading_lock;

                        float lrCoef[2];
                        uint8_t lrcoef_is_valid = false;

                        unsigned long lastTempRequest = 0; //last temperature request timestamp
                        const uint16_t temp_integration_delay = 750;

                        uint64_t temp_times[60];

                        float temperatures[60]; //moving window
                        int8_t t_head_index = 0;

                        bool t_ready = false;

                        void manageTemperatureSensor();
                        void sampleFill();
                        void getTemperatureTrend();

                public:

                        TemperatureSensing(OneWire * oneWire);
                        ~TemperatureSensing();

                        void start();
                        void stop();

                        float getLatestTemperature();
                        float getLinRegTemperature(float time);

                        bool isReady();
        };

}

#endif //TEMPERATURE_SENSOR_H