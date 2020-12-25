#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "PIT.h"
#include "PITConfig.h"

namespace PIT{

    Persistance::Persistance(){
        readConfig();
    }

    /**
     * Read configuration from non-volatile memory.
     * 
     * @param none
     * @return none
    */
    static void Persistance::readConfig(){
        uint16_t bringup_match = (uint16_t)EEPROM.read(0) << 8 | EEPROM.read(1);
        char * bp = (char*)&config;
        if(bringup_match == BRINGUP_CODE) //check to see if the codes match. if no then the NV memory needs to be initialized
        {
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
    static void Persistance::writeConfig(){
        char * bp = (char*)&config;
        for(uint8_t i = 0; i < sizeof(config); i++)   
            EEPROM.write(i+2, (uint8_t)bp[i]);

        uint16_t bringup_match = (uint16_t)EEPROM.read(0) << 8 | EEPROM.read(1);
        if(bringup_match != BRINGUP_CODE)
        {
            EEPROM.write(0, BRINGUP_CODE >> 8);
            EEPROM.write(1, BRINGUP_CODE & 0xFF);
        }   
    }

    /**
     * Reterive a copy of the configuration.
     * 
     * @param none
     * @return a copy of the active configuration
     */
    static PITConfig Persistance::getConfig(){        
        lock(cfg_lock){
            PITConfig p;
            p = config;
            return p;
        }
    }

    /**
     * Store a copy of the provided configuration in memory and write it to NV storage.
     * 
     * @param config The configuration to store
     * @return none
     */
    static void Persistance::setConfig(PITConfig config){
        lock(cfg_lock){
            Persistance::config = config;
            writeConfig(); 
        }
    }

}