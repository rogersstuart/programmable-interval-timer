#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

namespace PIT{

    class TemperatureSensor;
    class Display;

    class UI{

        private:



        public:

            //UI_home.cpp
            static void displayStatusLine(TemperatureSensor * sensor = NULL);
            static void idleDisplay();

            //UI_menus.cpp
            static void menuSelection();
            static void selectOption();
            static void selectOptionMenuItem1();
            static void selectOptionMenuItem2();
            static void selecOptionMenuItem3();
            static void setPulseWidthMenu();
            static void setPeriodWidthMenu();
            static void temperatureOptionsMenu();
            static void editMatchCondition();
            static void temperatureOptionMenuItem1();
            static void temperatureOptionMenuItem2();
            static void temperatureOptionMenuItem3();
            static void temperatureOptionMenuItem4();
            static void setTCEnable();
            static void setBlockingEnable();
            static void editSetpoint0();

            //UI_other.cpp
            static void showBootMessage(int delay_ms = 2000);
    };

}

#endif //USER_INTERFACE_H