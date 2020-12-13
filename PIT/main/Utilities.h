#ifndef PIT_UTILITIES_H
#define PIT_UTILITIES_H

class Utilities
{
    private:
    public:
            static String Utilities::generateTimeString(uint64_t total_seconds, uint8_t en_days, uint8_t zero_pad, uint8_t force_full_display);
};

#endif //PIT_UTILITIES_H