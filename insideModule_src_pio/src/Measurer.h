#ifndef Measurer_h
#define Measurer_h
#include <Arduino.h>
#include <DHT.h>
class Measurer {
public:
  virtual void readValues() = 0;
  virtual const char* getOutput();  //returns formatted string to display on OLED
  float getTemperature();
  int getHumidity();
  virtual ~Measurer();
  bool isInsideModule();  //1 -> inside; 0 -> outside
  int getIndex();
protected:
  float temperature = 0;
  int humidity = 0;
  const int OUTPUT_BUFFER_SIZE = 64;
  char* output;
  bool isInside;
  int index = 1;
};

#endif