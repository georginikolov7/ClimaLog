#ifndef InsideMeasurer_h
#define InsideMeasurer_h
#include "Measurer.h"
#include <DHT.h>

class InsideMeasurer : Measurer {
public:
InsideMeasurer(DHT* dhtSensor);
  bool readValues();
  const char* getOutput();
  float getTemperature();
  float getHumidity();
private:
  DHT* dhtSensor;
  
};

#endif