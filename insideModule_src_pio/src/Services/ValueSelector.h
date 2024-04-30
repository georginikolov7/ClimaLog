#ifndef ValueSelector_h
#define ValueSelector_h

#include "Models/IDisplay.h"
#include "Models/Button.h"

class ValueSelector {
public:
    ValueSelector(IDisplay* display, Button* setButton, const char* keyword = "value", const char* measureUnit = "");
    ~ValueSelector();
    int selectIntValue(int minValue, int maxValue, int startValue, int incrementValue);
    int selectStringValueFromArray( char array[][200], int arraySize, int incrementValue = 1); // returns the index of the selected string

private:
    const int BLINK_INTERVAL = 400;
    IDisplay* display;
    Button* setButton;

    const char* measureUnit;
    const char* keyword;
};
#endif