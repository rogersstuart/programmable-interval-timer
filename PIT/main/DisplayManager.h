#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "CustomChars.h"

class DisplayManager{
    private:
            LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

            const char stop_pause_str[] PROGMEM = " Stop     Pause ";
            const char stop_run_str[] PROGMEM = " Stop       Run ";
            const char run_str[] PROGMEM = "       Run      ";
            const char yes_no_str[] PROGMEM = "  Yes       No  ";
            const char match_less_set_str[] PROGMEM = "Match t < set   ";
            const char blank_line_str[] PROGMEM = "                ";

    public:
            void init();
            void showBootMessage(int delay = 4000);
};

#endif //DISPLAY_MANAGER_H