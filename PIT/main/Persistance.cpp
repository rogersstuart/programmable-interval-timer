#include "PIT.h"
#include "Persistance.h"
#include "EEPROM.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace PIT{

    Persistance::Persistance(){

        cfg_lock = xSemaphoreCreateMutex();
        xSemaphoreGive(cfg_lock);

        readConfig();
    }

    /**
     * Read configuration from non-volatile memory.
     * 
     * @param none
     * @return none
    */
    void Persistance::readConfig(){

        uint16_t bringup_match = (uint16_t)EEPROM.read(0) << 8 | EEPROM.read(1);
        char* bp = (char*)&config;
        if(bringup_match == BRINGUP_CODE){ //check to see if the codes match. if no then the NV memory needs to be initialized
        
            for(uint8_t i = 0; i < sizeof(PITConfig); i++)
                bp[i] = EEPROM.read(i+2);
        }
        else
            writeConfig();  
    }

    /**
     * Write configuration to non-volatile memory.
     * 
     * @param none
     * @return none
    */
    void Persistance::writeConfig(){

        char* bp = (char*)&config;
        for(uint8_t i = 0; i < sizeof(config); i++)   
            EEPROM.write(i+2, (uint8_t)bp[i]);

        uint16_t bringup_match = (uint16_t)EEPROM.read(0) << 8 | EEPROM.read(1);
        if(bringup_match != BRINGUP_CODE){

            EEPROM.write(0, BRINGUP_CODE >> 8);
            EEPROM.write(1, BRINGUP_CODE & 0xFF);
        }

        EEPROM.commit();  
    }

    Persistance& Persistance::getInstance(){

        static Persistance persistance;
        return persistance;
    }

    /**
     * Make a copy of the provided configuration and write it to NV storage.
     * 
     * @param config The configuration to store
     * @return none
     */
    void Persistance::setConfig(PITConfig config){

        auto instance = getInstance();
        
        xSemaphoreTake(instance.cfg_lock, 0xFFFF);
        instance.config = config;
        instance.writeConfig(); 
        xSemaphoreGive(instance.cfg_lock);
    }

    Persistance::PITConfig Persistance::getConfig(){

        auto instance = getInstance();
        
        xSemaphoreTake(instance.cfg_lock, 0xFFFF);
        auto config = instance.config;
        xSemaphoreGive(instance.cfg_lock);

        return config;
    }
}