#include <Arduino.h>
#include "Display/Display.h"
#include "UI.h"
#include <LiquidCrystal_I2C.h>

namespace PIT{

    void UI::showBootMessage(LiquidCrystal_I2C& lcd, int delay_ms){
    
        lcd.print(Display::version_str);
        lcd.setCursor(0, 1);
        lcd.print(Display::group_name_str);

        delay(delay_ms);
    }
}
