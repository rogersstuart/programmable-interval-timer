#include <Arduino.h>

namespace PIT{
    extern void init();
}

extern "C"{
  void app_main();
}

void app_main(){
    initArduino();
    PIT::init();
}