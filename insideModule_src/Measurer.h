#ifndef Measurer_h
#define Measurer_h
#include <Arduino.h>
#include <DHT.h>
class Measurer {
public:
  virtual bool readValues() = 0;
  virtual const char* getOutput();  //returns formatted string to display on OLED
  float getTemperature();
  float getHumidity();

protected:
  float temperature = 0;
  float humidity = 0;
};

#endif