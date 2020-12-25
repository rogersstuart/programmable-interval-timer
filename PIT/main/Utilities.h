#ifndef PIT_UTILITIES_H
#define PIT_UTILITIES_H

#include <Arduino.h>
#include <lock.h>

namespace PIT{

    class Utilities{

        private:

            inline static uint64_t system_uptime = 0;
            inline static _lock_t uptime_access_lock;

        public:

            static string generateTimeString(uint64_t total_seconds, uint8_t en_days, uint8_t zero_pad, uint8_t force_full_display);
            static uint8_t* timeBreakdown(uint64_t * seconds);
            static uint64_t getSystemUptime();

    };

}

#endif //PIT_UTILITIES_H