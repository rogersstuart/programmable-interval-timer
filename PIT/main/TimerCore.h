#ifndef TIMER_CORE_H
#define TIMER_CORE_H

//shabby hack

class TemperatureSensing;

namespace{
    typedef char RUN_MODE;
}

namespace PIT{

    class TimerCore{
        private:
        public:

            static void TimerCore::setMode(RUN_MODE new_mode);
            static void TimerCore::processTimer();

    };
}

#endif //TIMER_CORE_H

