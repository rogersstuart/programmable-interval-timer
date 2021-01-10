#include <Arduino.h>
#include "Display/Display.h"
#include "UI.h"
#include <LiquidCrystal_I2C.h>

namespace PIT{

    void UI::showBootMessage(LiquidCrystal_I2C& lcd, int delay_ms){
    
        lcd.backlight();

        lcd.setCursor(0, 0);
        lcd.print(Display::version_str);
        lcd.setCursor(0, 1);

        const char s[] = "Atomic";
        const char s1[] = "FRENZY";

        uint8_t size = sizeof(s) + sizeof(s1);
        uint8_t start = (16 - (size-2)) / 2;

        for(int k = 0; k < size-2; k++)
        {
            if(k < (sizeof(s)-1))
            {
                lcd.setCursor(k+start, 1);
                lcd.print(s[k]);
            }
            else
            {
                for(int i = 0; i < 2; i++)
                {
                    lcd.setCursor(k+start, 1);
                    lcd.write((uint8_t)rand() % 255); 
                    delay(1000/6);
                }
                
                lcd.setCursor(k+start, 1);
                lcd.print(s1[k-sizeof(s)+1]);
            }
        }

        delay(delay_ms);

        lcd.clear();

    }
}
