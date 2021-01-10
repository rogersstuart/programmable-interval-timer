/*
 * CustomChars.h
 *
 * Created: 6/4/2016 1:29:51 AM
 *  Author: Stuart
 */ 

#ifndef CUSTOMCHARS_H_
#define CUSTOMCHARS_H_

namespace PIT{

    

PROGMEM const char tdown[8] = {
    0b00000,
    0b01110,
    0b00100,
    0b00100,
    0b00000,
    0b11111,
    0b01110,
    0b00100
};

PROGMEM const char tup[8] = {
    0b00100,
    0b01110,
    0b11111,
    0b00000,
    0b01110,
    0b00100,
    0b00100,
    0b00000
};

PROGMEM const char tlvl[8] = {
    0b00000,
    0b01110,
    0b00100,
    0b00100,
    0b00000,
    0b00000,
    0b11111,
    0b00000
};

PROGMEM const char runTmpTmr_norm[8] = {
    0b00111,
    0b10010,
    0b11010,
    0b11100,
    0b11000,
    0b10111,
    0b00010,
    0b00111
};

PROGMEM const char runTmpTmr_trig[8] = {
    0b11000,
    0b01101,
    0b00101,
    0b00011,
    0b00111,
    0b01000,
    0b11101,
    0b11000
};

PROGMEM const char playChar[8] = {
    0b10000,
    0b11000,
    0b11100,
    0b11000,
    0b10000,
    0b00111,
    0b00010,
    0b00111
};

PROGMEM const char playTempEn[8] = {
    0b10000,
    0b11000,
    0b11100,
    0b11000,
    0b10000,
    0b00111,
    0b00010,
    0b00010
};

PROGMEM const char pauseChar[8] = {
    0b00000,
    0b01010,
    0b01010,
    0b01010,
    0b01010,
    0b01010,
    0b00000,
    0b00000
};

PROGMEM const char stopChar[8] = {
    0b00000,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b00000,
    0b00000
};

PROGMEM const char sampleFillChar[8] = {
    0b11000,
    0b10000,
    0b11000,
    0b01011,
    0b11010,
    0b00011,
    0b00010,
    0b00010
};

/*
enum CUSTOM_CHARS {
    
    TEMPERATURE_DOWNTREND,
    TEMPERATURE_UPTREND,
    TEMPERATURE_STEADY,
    TEMPERATURE_MODE,
    TEMPERATURE_TIMER_MODE,
    TIMER_RUN,
    TEMP_MODE_RUN,
    PAUSED,
    STOPPED,
    SAMPLE_FILL
};

PROGMEM const char * getCustomChar(CUSTOM_CHARS char_type){
    switch(char_type){
        case 0: return tdown;

    }
}
*/

}

#endif /* CUSTOMCHARS_H_ */