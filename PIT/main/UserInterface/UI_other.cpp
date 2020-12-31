#include <Arduino.h>
#include "Display/Display.h"
#include "UI.h"

namespace PIT{

    void UI::showBootMessage(LiquidCrystal_I2C& lcd, int delay_ms){
    
        auto display = Display::getInstance();
        auto lcd = display.checkOut();
        
        lcd.print(Display::version_str);
        lcd.setCursor(0, 1);
        lcd.print(Display::group_name_str);

        delay(delay_ms);
    }
}
