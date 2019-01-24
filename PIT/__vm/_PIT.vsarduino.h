/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino Pro or Pro Mini w/ ATmega328 (5V, 16 MHz), Platform=avr, Package=arduino
*/

#define __AVR_ATmega328p__
#define __AVR_ATmega328P__
#define ARDUINO 158
#define ARDUINO_MAIN
#define F_CPU 16000000L
#define __AVR__
#define F_CPU 16000000L
#define ARDUINO 158
#define ARDUINO_AVR_PRO
#define ARDUINO_ARCH_AVR
extern "C" void __cxa_pure_virtual() {;}

//
//
void processSerial();
void manageTemperatureSensor();
void LCDPrint_P(const char str[]);
void displayStatusLine();
float getLatestTemperature();
float getLinRegTemperature(float time);
void getTemperatureTrend();
void simpLinReg(float* x, float* y, float* lrCoef, uint8_t num);
uint64_t getSystemUptime();
int8_t getButtonPress();
void processTimer();
uint8_t tcheck();
void idleDisplay();
void setMode(uint8_t new_mode);
uint16_t getscaledPotValue(uint16_t max);
void updatePotReading();
void buttonPressDetection();
void readConfig();
void writeConfig();
void menuSelection();
void selectOption();
uint8_t* timeBreakdown(uint64_t * seconds);
String generateTimeString(uint64_t total_seconds, uint8_t en_days, uint8_t zero_pad, uint8_t force_full_display);
void selectOptionMenuItem1();
void selectOptionMenuItem2();
void selecOptionMenuItem3();
void setPulseWidthMenu();
void setPeriodWidthMenu();
void temperatureOptionsMenu();
void editMatchCondition();
void temperatureOptionMenuItem1();
void temperatureOptionMenuItem2();
void temperatureOptionMenuItem3();
void temperatureOptionMenuItem4();
void setTCEnable();
void setBlockingEnable();
void editSetpoint0();

#include "C:\Users\Stuart\Desktop\arduino-1.5.8\hardware\arduino\avr\variants\eightanaloginputs\pins_arduino.h" 
#include "C:\Users\Stuart\Desktop\arduino-1.5.8\hardware\arduino\avr\cores\arduino\arduino.h"
#include <..\PIT\PIT.ino>
#include <..\PIT\CustomChars.h>
#include <..\PIT\Poly.cpp>
#include <..\PIT\Poly.h>
