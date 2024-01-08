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
  virtual ~Measurer();
  bool getLocation();  //1 -> inside; 0 -> outside
protected:
  float temperature = 0;
  float humidity = 0;
  const int OUTPUT_BUFFER_SIZE = 50;
  char* output = new char[OUTPUT_BUFFER_SIZE];
  bool isInside;
};

#endif