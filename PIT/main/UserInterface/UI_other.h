#include <Arduino.h>
#include "Display.h"
#include "UI.h"

namespace PIT{

    void UI::showBootMessage(int delay_ms){
    
        auto display = Display::getInstance();
        auto lcd = display.checkOut();
        
        lcd.print(Display::version_str);
        lcd.setCursor(0, 1);
        lcd.print(Display::group_name_str);

        delay(delay_ms);

        display.checkIn(lcd); //needs to occur after the delay to guarantee that the screen is blocked for the duration of the message
    }
}
