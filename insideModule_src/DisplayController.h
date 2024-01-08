#ifndef DisplayController_h
#define DisplayController_h

//include the display library:
#include "Display.h"
#include "Measurer.h"

class DisplayController {
public:
  DisplayController(Display* display, Measurer** measurers, int measurersCount);
  void changeDisplayMode();
  void displayData();
  int getIterator();
private:
  Measurer** measurers;
  Display* display;
  int iterator = 0;
  int measurersCount = 0;
};
#endif