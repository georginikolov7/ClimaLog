#include <stdint.h>
#ifndef OutsideMeasurer_h
#define OutsideMeasurer_h
#include <EEPROM.h>
#include <RF24.h>
#include <nRF24L01.h>
#include "Measurer.h"
#include "Button.h"
#include "Display.h"

struct ReceiveBuffer
{
  int32_t humidity;
  int32_t batteryLevel;
  float temperature;
  float measuredDistance;
};

class OutsideMeasurer : public Measurer
{
public:
  OutsideMeasurer(RF24 *radio, int index);
  ~OutsideMeasurer();
  void setMountingHeight(int newHeight);
  void heightSetup(Button *setButton, Display *display);
  const char *getOutput() override;
  int getMountingHeight()
  {
    return mountingHeight;
  }
  int getMinHeight()
  {
    return MIN_HEIGHT;
  }
  int getMaxHeight()
  {
    return MAX_HEIGHT;
  }
  int getBatteryLevel()
  {
    return batteryLevel;
  }
  bool batLevelIsLow()
  {
    return batteryLevel <= LOW_BATTERY_PERCENTAGE; // returns true if battery is at capacity below 30%
  }
  void readValues() override;

  int getSnowDepth();

private:
  RF24 *radio;
  int snowDepth = 0;
  int batteryLevel = 0;       // outside module battery level in %
  int mountingHeight = 0;     // set mounting height in cm
  const int MAX_HEIGHT = 110; // maximum mounitng height in cm
  const int MIN_HEIGHT = 50;  // minimal mounting height in cm
  const int LOW_BATTERY_PERCENTAGE = 30;
  char *outsideOutput;
};
#endif