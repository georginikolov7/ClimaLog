#ifndef Measurer_h
#define Measurer_h
#include <Arduino.h>
#include <DHT.h>
class Measurer {
public:
    Measurer() = default;
    Measurer(const char* name);
    virtual ~Measurer();
    virtual void readValues() = 0;
    virtual const char* getOutput(); // returns formatted string to display on OLED
    const char* getTemperature();
    const char* getHumidity();
    const char* getName() const
    {
        return name;
    }
    int getIncorrectMeasureConstant()
    {
        // get the values which is set to incorrect measures
        return INCORRECT_MEASURE_VALUE;
    }

protected:
    const static int INCORRECT_MEASURE_VALUE = -42; // if any of the measures is equal to this value=> measure is not successfull (used for validating the data)
    char temperature[10] = "noVal";
    char humidity[10] = "noVal";
    const static int OUTPUT_BUFFER_SIZE = 256;
    char* output;
    const char* name;
};

#endif