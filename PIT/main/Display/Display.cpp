#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "Display.h"
#include "CustomChars.h"

namespace PIT{

    Display::Display(){

        lcd = new LiquidCrystal_I2C(0x27, 16, 2);
        
        vSemaphoreCreateBinary(*lcd_lock)
        xSemaphoreGive(*lcd_lock);
        
        (*lcd).begin (16,2);

        (*lcd).createChar(0, playChar);
        (*lcd).createChar(1, pauseChar);
        (*lcd).createChar(2, stopChar);
        (*lcd).createChar(3, playTempEn);
        (*lcd).createChar(4, runTmpTmr_norm);
        (*lcd).createChar(5, tlvl);
        (*lcd).createChar(6, tup);
        (*lcd).createChar(7, tdown);

        (*lcd).backlight();
        
        goHome((*lcd));// go home
    }

    inline void Display::goHome(LiquidCrystal_I2C& lcd, bool en_blink){

        if(en_blink)
            lcd.noBlink();
        
        lcd.setCursor(0, 0);
    }

    inline void Display::LCDPrint_P(LiquidCrystal_I2C& lcd, const char str[]){

        char c;
        if(!str)
            return;

        while((c = pgm_read_byte(str++)))
            lcd.print(c);
    }

    LiquidCrystal_I2C& Display::checkOut(){

        xSemaphoreTake(*lcd_lock, 0xFFFF);
        return *lcd;
    }

    void Display::checkIn(LiquidCrystal_I2C& lcd){

        xSemaphoreGive(*lcd_lock);
    }

    Display& Display::getInstance(){

        static Display display;
        return display;
    }
}

