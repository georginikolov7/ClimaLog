#ifndef OutsideMeasurer_h
#define OutsideMeasurer_h
#include <EEPROM.h>
#include "Radio.h"
#include <RH_ASK.h>
#include "Measurer.h"
#include "Button.h"
#include "Display.h"
class OutsideMeasurer : public Measurer {
public:
  OutsideMeasurer(Radio* radio, int index);
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
  bool readValues();

  int getSnowDepth();
private:
  Radio* radio;
  int snowDepth = 0;
  int mountingHeight = 0;      //set mounting height in cm
  const int MAX_HEIGHT = 110;  //maximum mounitng height in cm
  const int MIN_HEIGHT = 50;   //minimal mounting height in cm
  const int OUTSIDE_OUTPUT_BUFFER_SIZE = OUTPUT_BUFFER_SIZE + 10;
  char* outsideOutput = new char[OUTSIDE_OUTPUT_BUFFER_SIZE];
};
#endif