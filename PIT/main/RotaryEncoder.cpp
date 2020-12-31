#include <Arduino.h>
#include "RotaryEncoder.h"
#include "PIT.h"

typedef void (*voidFuncPtrArg)(void*);

extern "C"
{
	extern void __attachInterruptFunctionalArg(uint8_t pin, voidFuncPtrArg userFunc, void * arg, int intr_type, bool functional);
}

void IRAM_ATTR RotaryEncoder::readEncoder_ISR(){
    
	portENTER_CRITICAL_ISR(&(mux));
	if (isEnabled) {
		old_AB <<= 2;                   //remember previous state
		int8_t ENC_PORT = ((digitalRead(encoderBPin)) ? (1 << 1) : 0) | ((digitalRead(encoderAPin)) ? (1 << 0) : 0);	
		old_AB |= ( ENC_PORT & 0x03 );  //add current state
		encoder0Pos += ( enc_states[( old_AB & 0x0f )]);	

		if (encoder0Pos > (_maxEncoderValue))
			encoder0Pos = _circleValues ? _minEncoderValue : _maxEncoderValue;
		if (encoder0Pos < (_minEncoderValue))
			encoder0Pos = _circleValues ? _maxEncoderValue : _minEncoderValue;		
	}
	portEXIT_CRITICAL_ISR(&(mux));
}

RotaryEncoder::RotaryEncoder(uint8_t encoder_APin, uint8_t encoder_BPin, uint8_t encoderSteps) : 
    encoderAPin{encoder_APin},encoderBPin{encoder_BPin},encoderSteps{encoderSteps}{

	old_AB = 0;

	pinMode(encoderAPin, INPUT);
	pinMode(encoderBPin, INPUT);

	lastReadEncoder0Pos = 0;

	__attachInterruptFunctionalArg (digitalPinToInterrupt(encoderAPin), [](void* capture){

		auto scope = (RotaryEncoder*)capture;
		scope->readEncoder_ISR();

	}, this, CHANGE, true);

	__attachInterruptFunctionalArg (digitalPinToInterrupt(encoderBPin), [](void* capture){

		auto scope = (RotaryEncoder*)capture;
		scope->readEncoder_ISR();

	}, this, CHANGE, true);

	isEnabled = true;
}

void RotaryEncoder::setBoundaries(int16_t minEncoderValue, int16_t maxEncoderValue, bool circleValues){

	_minEncoderValue = minEncoderValue * encoderSteps;
	_maxEncoderValue = maxEncoderValue * encoderSteps;
	_circleValues = circleValues;
}

int16_t RotaryEncoder::readEncoder(){

	return (encoder0Pos / encoderSteps);
}

int16_t RotaryEncoder::encoderChanged(){

	int16_t _encoder0Pos = readEncoder();
	int16_t encoder0Diff = _encoder0Pos - lastReadEncoder0Pos;

	lastReadEncoder0Pos = _encoder0Pos;

	return encoder0Diff;
}

void RotaryEncoder::reset(int16_t newValue_){

	newValue_ = newValue_ * encoderSteps;
	encoder0Pos = newValue_;
	if (encoder0Pos > _maxEncoderValue) encoder0Pos = _circleValues ? _minEncoderValue : _maxEncoderValue;
	if (encoder0Pos < _minEncoderValue) encoder0Pos = _circleValues ? _maxEncoderValue : _minEncoderValue;	
}
