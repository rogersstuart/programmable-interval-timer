#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <Arduino.h>
#include "Persistance.h"
#include "RotaryEncoder.h"
#include <LiquidCrystal_I2C.h>
#include "TemperatureSensing.h"
#include "Display/Display.h"

#define SHORT_PRESS_DURATION 100
#define LONG_PRESS_DURATION 1000
#define NUISANCE_PRESS_DURATION 10000

#define BUTTON_READ_BLOCK_DURATION 1000/30

#define NUISANCE_PRESS -1
#define SHORT_PRESS 0
#define LONG_PRESS 1

namespace PIT{

    namespace{

        typedef char RUN_MODE; //maybe enum later
        typedef signed char PRESS_TYPE;
        
    }

    class UI{

        private:

            static inline RotaryEncoder encoder{};

        public:

            //UI_home.cpp
            static void displayStatusLine(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd, TemperatureSensing * sensor);
            static void idleDisplay(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd, TemperatureSensing * sensor);

            //UI_menus.cpp
            static void menuSelection(PRESS_TYPE button_press, Persistance::PITConfig& config, LiquidCrystal_I2C& lcd);
            static void selectOption(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd);
            static void selectOptionMenuItem1(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd);
            static void selectOptionMenuItem2(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd);
            static void selecOptionMenuItem3(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd);
            static void setPulseWidthMenu(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd);
            static void setPeriodWidthMenu(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd);
            static void temperatureOptionsMenu(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd);
            static void editMatchCondition(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd);
            static void temperatureOptionMenuItem1(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd);
            static void temperatureOptionMenuItem2(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd);
            static void temperatureOptionMenuItem3(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd);
            static void temperatureOptionMenuItem4(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd);
            static void setTCEnable(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd);
            static void setBlockingEnable(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd);
            static void editSetpoint0(Persistance::PITConfig& config, LiquidCrystal_I2C& lcd);

            //UI_other.cpp
            static void showBootMessage(LiquidCrystal_I2C& lcd, int delay_ms = 500);

            //UI_button.cpp
            static PRESS_TYPE getButtonPress();
    };

}

#endif //USER_INTERFACE_H