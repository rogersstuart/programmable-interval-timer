#include "Utilities.h"

namespace PIT{

    static string Utilities::generateTimeString(uint64_t total_seconds, uint8_t en_days, uint8_t zero_pad, uint8_t force_full_display){
    
        uint8_t * tbreak = timeBreakdown(&total_seconds);

        String str = "";

        if(tbreak[0] > 0 && en_days)
            str += String(tbreak[0]) + ":";

        if(tbreak[1] > 0 || (tbreak[0] > 0 && en_days) || force_full_display)
        {
            if((zero_pad || force_full_display) && tbreak[1] < 10)
                str += "0";

            str +=  String(tbreak[1]) + ":";
        }

        if(tbreak[2] > 0 || (tbreak[1] > 0 || (tbreak[0] > 0 && en_days)) || force_full_display)
        {
            if((zero_pad || force_full_display) && tbreak[2] < 10)
                str += "0";

            str +=  String(tbreak[2]) + ":";
        }

        if((zero_pad || force_full_display) && tbreak[3] < 10)
            str += "0";
        
        str +=  String(tbreak[3]);

        return str;   
    }

    static uint8_t* Utilities::timeBreakdown(uint64_t * seconds)
    {
        static uint8_t tbreak[4];
        
        tbreak[0] = *seconds/seconds_in_day;
        tbreak[1] = (*seconds%seconds_in_day)/seconds_in_hour;
        tbreak[2] = ((*seconds%seconds_in_day)%seconds_in_hour)/seconds_in_minute;
        tbreak[3] = ((*seconds%seconds_in_day)%seconds_in_hour)%seconds_in_minute;

        return (uint8_t*)&tbreak;
    }

    static uint64_t Utilities::getSystemUptime(){

        system_uptime += (uint64_t)((long)millis()-(uint32_t)system_uptime);
        return system_uptime;
    }
}