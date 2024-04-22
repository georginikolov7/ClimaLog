#ifndef ValueSelector_h
#define ValueSelector_h

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "Models/Button.h"

class ValueSelector {
public:
    ValueSelector(Adafruit_SSD1306* display, Button* setButton, const char* keyword = "value", const char* measureUnit = "");
    ~ValueSelector();
    int selectIntValue(int minValue, int maxValue, int startValue, int incrementValue);
    int selectStringValueFromArray( char array[][200], int arraySize, int incrementValue = 1); // returns the index of the selected string

private:
    const int BLINK_INTERVAL = 400;
    Adafruit_SSD1306* display;
    Button* setButton;

    char* measureUnit;
    char* keyword;
};
#endif