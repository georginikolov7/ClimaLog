#ifndef OutsideMeasurer_h
#define OutsideMeasurer_h
#include <EEPROM.h>
#include <RF24.h>
#include <nRF24L01.h>
#include "Measurer.h"
#include "Button.h"
#include "Display.h"

struct ReceiveBuffer {
  float temperature;
  int humidity;
  float measuredDistance;
  int batteryLevel;
};
class OutsideMeasurer : public Measurer {
public:
  OutsideMeasurer(RF24* radio, int index);
  ~OutsideMeasurer();
  void setMountingHeight(int newHeight);
  void heightSetup(Button* setButton, Display* display);
  const char* getOutput() override;
  int getMountingHeight() {
    return mountingHeight;
  }
  int getMinHeight() {
    return MIN_HEIGHT;
  }
  int getMaxHeight() {
    return MAX_HEIGHT;
  }
  int getBatteryLevel() {
    return batteryLevel;
  }
  void readValues() override;

  int getSnowDepth();
private:
  RF24* radio;
  int snowDepth = 0;
  int batteryLevel = 0;        //outside module battery level in %
  int mountingHeight = 0;      //set mounting height in cm
  const int MAX_HEIGHT = 110;  //maximum mounitng height in cm
  const int MIN_HEIGHT = 50;   //minimal mounting height in cm
  const int OUTSIDE_OUTPUT_BUFFER_SIZE = OUTPUT_BUFFER_SIZE + 10;
  char* outsideOutput = new char[OUTSIDE_OUTPUT_BUFFER_SIZE];
};
#endif