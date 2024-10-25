#include "ValueSelector.h"
#include <cstring>

ValueSelector::ValueSelector(IDisplay* display, Button* setButton, const char* keyword, const char* measureUnit)
{
    this->display = display;
    this->setButton = setButton;
    this->keyword = keyword;
    this->measureUnit = measureUnit;
    // int keywordLength = strlen(keyword);
    // this->keyword = new char[keywordLength + 1];
    // std::strcpy(this->keyword, keyword);
    // int measureUnitLength = strlen(measureUnit);
    // this->measureUnit = new char[measureUnitLength + 1];
    // std::strcpy(this->measureUnit, measureUnit);
}
ValueSelector::~ValueSelector()
{
    // delete[] keyword;
    // delete[] measureUnit;
}

int ValueSelector::selectIntValue(int minValue, int maxValue, int startValue, int incrementValue)
{
    int currentValue = startValue;
    unsigned long lastBlinkTime = 0; // used to store last time display was blinked
    bool isOn = false; // used to determine whether display is showing value string (in blinker function)

    while (!setButton->isLongPressed()) {
        if (setButton->isShortPressed()) {
            currentValue += incrementValue;
        }
        if (currentValue > maxValue) {
            currentValue = minValue;
        }

        char text_value[32] = "\0";
        char text_noValue[32] = "\0";
        sprintf(text_value, "Select \n%s:\n%i %s", keyword, currentValue, measureUnit);
        sprintf(text_noValue, "Select \n%s:", keyword);

        if (millis() - lastBlinkTime > BLINK_INTERVAL) {
            if (isOn) {
                display->resetDisplay();
                display->writeText(text_noValue);
                isOn = false;
                delay(10);
            } else {
                display->resetDisplay();
                display->writeText(text_value);
                isOn = true;
                delay(10);
            }
            lastBlinkTime = millis();
        }
    }
    char text[64] = "\0";
    sprintf(text, "Selected \n%s\n   %i %s", keyword, currentValue, measureUnit);

    display->resetDisplay();
    display->writeText(text);
    delay(2500);

    return currentValue;
}

int ValueSelector::selectStringValueFromArray(char array[][200], int arraySize, int incrementValue)
{
    int currentValue = 0;
    unsigned long lastBlinkTime = 0; // used to store last time display was blinked
    bool isOn = false; // used to determine whether display is showing value string (in blinker function)

    while (!setButton->isLongPressed()) {
        if (setButton->isShortPressed()) {
            currentValue += incrementValue;
        }
        if (currentValue >= arraySize) {
            currentValue = 0;
        }

        char text_value[32] = "\0";
        char text_noValue[32] = "\0";
        sprintf(text_value, "Select \n%s:\n%s", keyword, array[currentValue]);
        sprintf(text_noValue, "Select \n%s:", keyword);

        if (millis() - lastBlinkTime > BLINK_INTERVAL) {
            if (isOn) {
                display->resetDisplay();
                display->writeText(text_noValue);
                isOn = false;
                delay(10);
            } else {
                display->resetDisplay();
                display->writeText(text_value);
                isOn = true;
                delay(10);
            }
            lastBlinkTime = millis();
        }
    }
    char text[64] = "\0";
    sprintf(text, "Selected \n%s\n  %s %s", keyword, array[currentValue], measureUnit);
    display->resetDisplay();
    display->writeText(text);
    delay(2500);

    return currentValue;
}