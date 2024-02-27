#ifndef DisplayController_h
#define DisplayController_h

// include the display library:
#include "Display.h"
#include "InsideMeasurer.h"
#include "Measurer.h"
#include "OutsideMeasurer.h"

class DisplayController {
public:
    DisplayController(Display* display, InsideMeasurer* insideMeasurer, OutsideMeasurer** outsideMeasurers, int outsideMeasurersCount);
    ~DisplayController();
    void changeDisplayMode();
    void displayData();
    int getCurrentIndex();

private:
    Measurer** measurers;
    OutsideMeasurer** outsideMeasurers;
    Display* display;
    int index = 0;
    int measurersCount = 0;
};
#endif