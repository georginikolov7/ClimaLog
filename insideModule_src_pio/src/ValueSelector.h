#ifndef ValueSelector_h
#define ValueSelector_h

#include "Button.h"
#include "Display.h"

class ValueSelector {
public:
    ValueSelector(Display* display, Button* setButton, const char* keyword = "value", const char* measureUnit = "");
    ~ValueSelector();
    int selectIntValue(int minValue, int maxValue, int startValue, int incrementValue);
    int selectStringValueFromArray(const char (*array)[50], int arraySize, int incrementValue = 1); // returns the index of the selected string

private:
    const int BLINK_INTERVAL = 400;
    Display* display;
    Button* setButton;

    char* measureUnit;
    char* keyword;
};
#endif