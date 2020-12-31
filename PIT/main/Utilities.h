#ifndef PIT_UTILITIES_H
#define PIT_UTILITIES_H

class String;

namespace PIT{

    namespace{

        //typedef unsigned long long uint64_t;
        //typedef unsigned char uint8_t;
    }

    class Utilities{

        private:

            inline static uint64_t system_uptime = 0;

        public:

            static String generateTimeString(uint64_t total_seconds, uint8_t en_days, uint8_t zero_pad, uint8_t force_full_display);
            static uint8_t* timeBreakdown(uint64_t * seconds);
            static uint64_t getSystemUptime();
    };

}

#endif //PIT_UTILITIES_H