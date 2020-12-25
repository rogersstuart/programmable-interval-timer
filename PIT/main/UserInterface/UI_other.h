#include <Arduino.h>
#include "Display.h"
#include "UI.h"

namespace PIT{

    void UI::showBootMessage(int delay_ms){
    
        auto display = Display::getInstance().checkOut();
        
        display.print(Display::version_str);
        display.setCursor(0, 1);
        display.print(Display::group_name_str);

        delay(delay_ms);
    }
}
