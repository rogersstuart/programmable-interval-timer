#include "DisplayManager.h"

void DisplayManager::init()
{
    lcd.begin (16,2);

    lcd.createChar(0, playChar);
    lcd.createChar(1, pauseChar);
    lcd.createChar(2, stopChar);
    lcd.createChar(3, playTempEn);
    lcd.createChar(4, runTmpTmr_norm);
    lcd.createChar(5, tlvl);
    lcd.createChar(6, tup);
    lcd.createChar(7, tdown);

    lcd.backlight();
    lcd.home ();// go home
}

void DisplayManager::showBootMessage(int delay)
{
    lcd.print(F("PIT 1.1.20"));
    lcd.setCursor(0, 1);
    lcd.print(F("MST LLC 2016"));

    delay(wait);
}