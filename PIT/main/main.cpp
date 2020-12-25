#include <Arduino.h>

namespace PIT{
    extern void init();
}

void app_main()
{
    initArduino();
    PIT::init();
}