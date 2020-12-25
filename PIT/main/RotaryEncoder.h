// RotaryEncoder.h
// based on https://github.com/marcmerlin/IoTuz code - extracted and modified Encoder code
// based on https://github.com/igorantolic/ai-esp32-rotary-encoder

#ifndef ROTARTY_ENCODER_H
#define ROTARTY_ENCODER_H

class RotaryEncoder{
    private:

        portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
        volatile int16_t encoder0Pos = 0;
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

        RotaryEncoder(uint8_t encoderAPin, uint8_t encoderBPin, uint8_t encoderSteps);

        void setBoundaries(int16_t minValue = -100, int16_t maxValue = 100, bool circleValues = false);
        void IRAM_ATTR readEncoder_ISR();
        
        void setup(void (*ISR_callback)());
        void begin();
        void reset(int16_t newValue = 0);
        void enable();
        void disable();
        int16_t readEncoder();
        int16_t encoderChanged();
};

#endif //ROTARTY_ENCODER_H