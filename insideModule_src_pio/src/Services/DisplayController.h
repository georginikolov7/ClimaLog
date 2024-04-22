#ifndef DisplayController_h
#define DisplayController_h

// include the display library:
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#include "Models/InsideMeasurer.h"
#include "Models/Measurer.h"
#include "Models/OutsideMeasurer.h"
#include "Repos/OutsideMeasurersRepo.h"

class DisplayController {
public:
    DisplayController(Adafruit_SSD1306* display);
    void begin(InsideMeasurer* insideMeasurer, OutsideMeasurersRepo* outsideMeasurers, int outsideMeasurersCount);
    ~DisplayController();
    void changeDisplayMode();
    void displayData();
    int getCurrentIndex();

private:
    bool ready = false;
    Measurer** measurers;
    OutsideMeasurersRepo* outsideMeasurers;
    Adafruit_SSD1306* display;
    int index = 0;
    int measurersCount = 0;

    // Display properties:
    const int DISPLAY_WIDTH = 128;
    const int DISPLAY_HEIGHT = 64;
};
#endif