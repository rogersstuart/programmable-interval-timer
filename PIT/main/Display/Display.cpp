#include "PIT.h"
#include <Arduino.h>
#include "Display.h"
#include "CustomChars.h"
#include <LiquidCrystal_I2C.h>

#include <freertos/task.h>

#include "FrequencyGenerator.h"

namespace PIT{

    Display::Display(){

        //start the contrast adjustment f_gen
        static fgen_info_t       info;
        static fgen_resources_t* fgen;

        fgen_info( 20000.0, 
                0.8, 
                &info);

        fgen = fgen_alloc(&info, (gpio_num_t)12);
        printf("Channel: %02d \tGPIO: %02d\tFreq.: %0.2f Hz\tBlocks: %d\n", 
                fgen->channel, fgen->gpio_num, fgen->info.freq, fgen->info.mem_blocks);
        fgen_start(fgen);

        /////
        
        Wire.begin(DISPLAY_SDA_PIN, DISPLAY_SCL_PIN, 1000000);
        lcd.begin(16,2);

        lcd.createChar(0, playChar);
        lcd.createChar(1, pauseChar);
        lcd.createChar(2, stopChar);
        lcd.createChar(3, playTempEn);
        lcd.createChar(4, runTmpTmr_norm);
        lcd.createChar(5, tlvl);
        lcd.createChar(6, tup);
        lcd.createChar(7, tdown);

        lcd.backlight();
        lcd.leftToRight();
        lcd.noAutoscroll();
        
        lcd.clear();
        goHome(lcd);// go home

        lcd_lock = xSemaphoreCreateMutex();
        xSemaphoreGive(lcd_lock);
    }

    void Display::goHome(LiquidCrystal_I2C& lcd, bool en_blink){

        if(en_blink)
            lcd.noBlink();
        
        lcd.setCursor(0, 0);
    }

    void Display::LCDPrint_P(LiquidCrystal_I2C& lcd, const char str[]){

        char c;
        if(!str)
            return;

        while((c = pgm_read_byte(str++)))
            lcd.print(c);
    }

    LiquidCrystal_I2C& Display::checkOut(){

        xSemaphoreTake(lcd_lock, portMAX_DELAY);
            //vTaskDelay(1);

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

