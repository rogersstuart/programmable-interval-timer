#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

namespace PIT{

        class OneWire;
        class DallasTemperature;
        class DeviceAddress;

        namespace{
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
                        DeviceAddress tempDeviceAddress;

                        TaskHandle_t * task_handle;
                        _lock_t * one_wire_lock;

                        uint8_t lrcoef_is_valid = false;

                        unsigned long lastTempRequest = 0; //last temperature timestamp
                        const uint16_t temp_integration_delay = 750;

                        uint64_t temp_times[60];

                        float temperatures[60]; //moving window
                        int8_t temperatures_index = 0;

                        void manageTemperatureSensor();

                public:

                        TemperatureSensing(OneWire * oneWire, _lock_t * oneWireLock);
                        ~TemperatureSensing();
                        void start();
                        void stop();    
        };

}

#endif //TEMPERATURE_SENSOR_H