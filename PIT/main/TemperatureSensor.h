#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <OneWire.h>
#include <DallasTemperature.h>
#include "LinearRegression.h"

class TemperatureSensor
{
    private:
            
            OneWire * one_wire;
            DallasTemperature * sensors;
            DeviceAddress tempDeviceAddress;

            _lock_t * one_wire_lock;

            uint8_t lrcoef_is_valid = false;

            unsigned long lastTempRequest = 0;
            const uint16_t temp_integration_delay = 750;

            uint64_t temp_times[60];

            float temperatures[60];
            int8_t temperatures_index = 0;

            float lrCoef[2] = {0, 0};
    public:
            void TemperatureSensor(OneWire * oneWire, _lock_t oneWireLock);
            void start();
            void stop();
            
}

#endif //TEMPERATURE_SENSOR_H