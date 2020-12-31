#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "RotaryEncoder.h"
#include <tuple>

#define SHORT_PRESS_DURATION 100
#define LONG_PRESS_DURATION 1000
#define NUISANCE_PRESS_DURATION 10000

#define BUTTON_READ_BLOCK_DURATION 1000/30

#define NUISANCE_PRESS -1
#define SHORT_PRESS 0
#define LONG_PRESS 1

class LiquidCrystal_I2C;
class TemperatureSensing;
class Display;

class Persistance{
    class PITConfig;
};

namespace PIT{

    namespace{

        typedef char RUN_MODE; //maybe enum later
        typedef char PRESS_TYPE;
    }

    extern uint64_t uptime_at_cycle_start;
    extern uint64_t uptime_at_pause;
    extern RUN_MODE run_mode;
    extern bool button_press_detected;
    extern uint32_t press_detection_time;

    class UI{

        private:

            static inline RotaryEncoder encoder{};

        public:

            //UI_home.cpp
            static void displayStatusLine(PITConfig& config, LiquidCrystal_I2C& lcd, TemperatureSensing * sensor);
            static void idleDisplay(PITConfig& config, LiquidCrystal_I2C& lcd, TemperatureSensing * sensor);

            //UI_menus.cpp
            static void menuSelection(PRESS_TYPE button_press, PITConfig& config, LiquidCrystal_I2C& lcd);
            static void selectOption(PITConfig& config, LiquidCrystal_I2C& lcd);
            static void selectOptionMenuItem1(PITConfig& config, LiquidCrystal_I2C& lcd);
            static void selectOptionMenuItem2(PITConfig& config, LiquidCrystal_I2C& lcd);
            static void selecOptionMenuItem3(PITConfig& config, LiquidCrystal_I2C& lcd);
            static void setPulseWidthMenu(PITConfig& config, LiquidCrystal_I2C& lcd);
            static void setPeriodWidthMenu(PITConfig& config, LiquidCrystal_I2C& lcd);
            static void temperatureOptionsMenu(PITConfig& config, LiquidCrystal_I2C& lcd);
            static void editMatchCondition(PITConfig& config, LiquidCrystal_I2C& lcd);
            static void temperatureOptionMenuItem1(PITConfig& config, LiquidCrystal_I2C& lcd);
            static void temperatureOptionMenuItem2(PITConfig& config, LiquidCrystal_I2C& lcd);
            static void temperatureOptionMenuItem3(PITConfig& config, LiquidCrystal_I2C& lcd);
            static void temperatureOptionMenuItem4(PITConfig& config, LiquidCrystal_I2C& lcd);
            static void setTCEnable(PITConfig& config, LiquidCrystal_I2C& lcd);
            static void setBlockingEnable(PITConfig& config, LiquidCrystal_I2C& lcd);
            static void editSetpoint0(PITConfig& config, LiquidCrystal_I2C& lcd);

            //UI_other.cpp
            static void showBootMessage(LiquidCrystal_I2C& lcd, int delay_ms = 2000);

            //UI_button.cpp
            static PRESS_TYPE getButtonPress();
    };

}

#endif //USER_INTERFACE_H