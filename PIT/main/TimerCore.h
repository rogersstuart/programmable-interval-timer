#ifndef TIMER_CORE_H
#define TIMER_CORE_H

//shabby hack

namespace{
    typedef char RUN_MODE;
}

namespace PIT{

    class TimerCore{
        private:
        public:

            static void setMode(RUN_MODE new_mode);
            static void processTimer();

    };
}

#endif //TIMER_CORE_H

