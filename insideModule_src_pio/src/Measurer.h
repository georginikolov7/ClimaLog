#ifndef Measurer_h
#define Measurer_h
#include <Arduino.h>
#include <DHT.h>
class Measurer
{
public:
  Measurer(const char* name);
  virtual void readValues() = 0;
  virtual const char *getOutput(); // returns formatted string to display on OLED
  float getTemperature();
  int getHumidity();
  virtual ~Measurer();

protected:
  float temperature = 0;
  int humidity = 0;
  const int OUTPUT_BUFFER_SIZE = 256;
  char *output;
  const char *name;
};

#endif