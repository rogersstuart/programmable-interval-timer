// RotaryEncoder.h
// based on https://github.com/marcmerlin/IoTuz code - extracted and modified Encoder code
// based on https://github.com/igorantolic/ai-esp32-rotary-encoder

#ifndef ROTARTY_ENCODER_H
#define ROTARTY_ENCODER_H

#define ROTARY_ENCODER_A_PIN 32
#define ROTARY_ENCODER_B_PIN 33

#include <Arduino.h>

namespace PIT{

    typedef void (*voidFuncPtrArg)(void*);

    class RotaryEncoder{

        private:

            portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
            volatile double encoder0Pos = 0;
            bool _circleValues = false;
            bool isEnabled = true;

            uint8_t encoderAPin;
            uint8_t encoderBPin;
            uint8_t encoderSteps;

            int16_t _minEncoderValue = -1 << 15;
            int16_t _maxEncoderValue = 1 << 15;

            uint8_t old_AB;
            int16_t lastReadEncoder0Pos;

            int8_t enc_states[16] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
            void ISR_callback();

        public: 

            RotaryEncoder(uint8_t encoderAPin = ROTARY_ENCODER_A_PIN, uint8_t encoderBPin = ROTARY_ENCODER_B_PIN, uint8_t encoderSteps = 4);

            void setBoundaries(int16_t minValue = -100, int16_t maxValue = 100, bool circleValues = false);
            void IRAM_ATTR readEncoder_ISR();

            void reset(int16_t newValue = 0);

            int16_t readEncoder();
            int16_t encoderChanged();
    };
}

#endif //ROTARTY_ENCODER_H