#ifndef DisplayController_h
#define DisplayController_h

//include the display library:
#include "Display.h"
#include "Measurer.h"
#include "InsideMeasurer.h"
#include "OutsideMeasurer.h"

const int BATTERY_INDICATOR_WIDTH = 16;
const int BATTERY_INDICATOR_HEIGHT = 16;
const unsigned char batteryIndicator[] PROGMEM = {
  0xff, 0xff, 0xe1, 0xff, 0xe1, 0xff, 0x80, 0x7b, 0x9f, 0x7b, 0x9f, 0x69, 0x9f, 0x6d, 0x9f, 0x65,
  0x9f, 0x65, 0x9f, 0x6d, 0x9f, 0x69, 0x80, 0x7b, 0x80, 0x7b, 0x80, 0x7f, 0xc0, 0x7f, 0xff, 0xff
};
class DisplayController {
public:
  DisplayController(Display* display, InsideMeasurer* insideMeasurer, OutsideMeasurer** outsideMeasurers, int outsideMeasurersCount);
  ~DisplayController();
  void changeDisplayMode();
  void displayData();
  int getIterator();
private:
  Measurer** measurers;
  OutsideMeasurer** outsideMeasurers;
  Display* display;
  int iterator = 0;
  int measurersCount = 0;
};
#endif