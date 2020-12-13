#include "Utilities.h"

String Utilities::generateTimeString(uint64_t total_seconds, uint8_t en_days, uint8_t zero_pad, uint8_t force_full_display)
{
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