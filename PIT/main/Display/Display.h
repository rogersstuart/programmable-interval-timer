#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#define ENABLE_BLINK true
#define DISABLE_BLINK false

#define DISPLAY_SDA_PIN 21
#define DISPLAY_SCL_PIN 22

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

namespace PIT{

    class Display final{

        private:

            LiquidCrystal_I2C lcd{0x27, 40, 2};
            SemaphoreHandle_t lcd_lock;

            Display();

        public:

            inline static const char stop_pause_str[] PROGMEM = " Stop     Pause ";
            inline static const char stop_run_str[] PROGMEM = " Stop       Run ";
            inline static const char run_str[] PROGMEM = "       Run      ";
            inline static const char yes_no_str[] PROGMEM = "  Yes       No  ";
            inline static const char match_less_set_str[] PROGMEM = "Match t < set   ";
            inline static const char blank_line_str[] PROGMEM = "                ";

            inline static const char version_str[] PROGMEM = "  PIT_32 1.2.0";
            //inline static const char group_name_str[] PROGMEM = "AtomicFRENZY";

            static void LCDPrint_P(LiquidCrystal_I2C& lcd, const char str[]);
            static void goHome(LiquidCrystal_I2C& lcd, bool en_blink = true);

            LiquidCrystal_I2C& checkOut();
            void checkIn(LiquidCrystal_I2C& lcd);

            static Display& getInstance();
    };
}

#endif //DISPLAY_MANAGER_H