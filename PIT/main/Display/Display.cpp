#include <Arduino.h>

#include "Display.h"
#include "CustomChars.h"

namespace PIT{

    Display::Display(){

        vSemaphoreCreateBinary(lcd_lock)
        xSemaphoreGive(lcd_lock);
        
        lcd.begin (16,2);

        lcd.createChar(0, playChar);
        lcd.createChar(1, pauseChar);
        lcd.createChar(2, stopChar);
        lcd.createChar(3, playTempEn);
        lcd.createChar(4, runTmpTmr_norm);
        lcd.createChar(5, tlvl);
        lcd.createChar(6, tup);
        lcd.createChar(7, tdown);

        lcd.backlight();
        goHome();// go home
    }

    void Display::goHome(bool en_blink){

        xSemaphoreTake(lcd_lock, 0xFFFF);
        
        if(en_blink)
            lcd.noBlink();
        
        lcd.setCursor(0, 0);

        xSemaphoreGive(lcd_lock);
    }

    inline void Display::LCDPrint_P(const char str[]){

        xSemaphoreTake(lcd_lock, 0xFFFF);
        
        char c;
        if(!str)
            return;

        while((c = pgm_read_byte(str++)))
            lcd.print(c);

        xSemaphoreGive(lcd_lock);
    }

    LiquidCrystal_I2C& Display::checkOut(){

        xSemaphoreTake(lcd_lock, 0xFFFF);
        return lcd;
    }

    void Display::checkIn(LiquidCrystal_I2C& lcd){

        xSemaphoreGive(lcd_lock);
    }

    Display& Display::getInstance(){

        static Display display;
        return display;
    }
}
