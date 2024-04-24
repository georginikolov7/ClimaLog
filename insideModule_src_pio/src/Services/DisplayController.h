#ifndef DisplayController_h
#define DisplayController_h

// include the display library:
#include "Models/IDisplay.h"

#include "Models/InsideMeasurer.h"
#include "Models/Measurer.h"
#include "Models/OutsideMeasurer.h"
#include "Repos/OutsideMeasurersRepo.h"

class DisplayController {
public:
    DisplayController(IDisplay* display);
    void begin(InsideMeasurer* insideMeasurer, OutsideMeasurersRepo* outsideMeasurers);
    ~DisplayController();
    void changeDisplayMode();
    void displayData();
    int getCurrentIndex();

private:
    bool ready = false;
    OutsideMeasurersRepo* outsideMeasurers;
    InsideMeasurer* insideMeasurer;
    IDisplay* display;
    int index = 0;

    // Display properties:
    const int DISPLAY_WIDTH = 128;
    const int DISPLAY_HEIGHT = 64;
};
#endif