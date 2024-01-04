#ifndef OutsideMeasurer_h
#define OutsideMeasurer_h
#include <RH_ASK.h>
#include "Measurer.h"
class OutsideMeasurer : Measurer {
public:
  OutsideMeasurer(RH_ASK* radioDriver, int mountingDistance = 100);
  void setMountingDistance(int newDistance);
  const char* getOutput();
  bool readValues();
  float getTemperature();
  float getHumidity();
  int getSnowDepth();
private:
  RH_ASK* radioDriver;
  int snowDepth = 0;
  int mountingDistance = 0;
};
#endif