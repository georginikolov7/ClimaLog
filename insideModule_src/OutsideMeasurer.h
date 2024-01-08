#ifndef OutsideMeasurer_h
#define OutsideMeasurer_h
#include <EEPROM.h>
#include <RH_ASK.h>
#include "Measurer.h"
class OutsideMeasurer : public Measurer {
public:
  OutsideMeasurer(RH_ASK* radioDriver);
  ~OutsideMeasurer();
  void setMountingHeight(int newHeight);
  const char* getOutput() override;
  bool readValues();

  int getSnowDepth();
private:
  RH_ASK* radioDriver;
  int snowDepth = 0;
  int mountingHeight = 0;
  char* outsideOutput = new char[OUTPUT_BUFFER_SIZE + 10];
};
#endif