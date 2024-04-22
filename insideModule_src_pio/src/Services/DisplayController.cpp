#include "DisplayController.h"
#include "Icons.h"
#include "WiFi.h"
#include <Arduino.h>
#include <stdio.h>
#include <string.h>

DisplayController::DisplayController(IDisplay* display)

{
    this->display = display;
}

void DisplayController::begin(InsideMeasurer* insideMeasurer, OutsideMeasurersRepo* outsideMeasurers, int outsideMeasurersCount)
{
    this->outsideMeasurers = outsideMeasurers;
    const int measurersCount = outsideMeasurersCount + 1;
    this->measurers = new Measurer*[measurersCount];
    this->measurersCount = measurersCount;

    // 1st element is the insideMeasurer:
    measurers[0] = insideMeasurer;

    // Fill rest of array with outsideMeasurers:

    const OutsideMeasurer* outMeasurers = outsideMeasurers->getElements();
    for (int i = 0; i < outsideMeasurersCount; i++) {
        measurers[i + 1] = (Measurer*)&(outMeasurers[i]);
    }
    ready = true;
}
DisplayController::~DisplayController()
{
    delete[] measurers;
}

void DisplayController::changeDisplayMode()
{
    if (!ready) {
        Serial.println("Display controller not initialized!");
        return;
    }
    index = (index + 1) % measurersCount;
    // increments the iterator by 1. Keeps it inside the bounds of measurers array
}

void DisplayController::displayData()
{
    if (!ready) {
        throw;
    }

    if (index >= 1 && (*outsideMeasurers)[index - 1].batLevelIsLow()) {
        // size of outsideMeasurers array is measurers size - 1 => index is iterator - 1

        // Draw the bitmap on the display:
        int xPosition = DISPLAY_WIDTH - BATTERY_INDICATOR_WIDTH;
        display->drawBitmap(xPosition, 0, batteryIndicator, BATTERY_INDICATOR_WIDTH, BATTERY_INDICATOR_HEIGHT);
    }

    if (WiFi.status() != WL_CONNECTED) {
        // Display WiFi icon below battery icon:
        display->drawBitmap(DISPLAY_WIDTH - NO_WIFI_WIDTH, BATTERY_INDICATOR_HEIGHT + 5, epd_bitmap_no_wifi, NO_WIFI_WIDTH, NO_WIFI_HEIGHT);
    }
    display->resetDisplay();
    display->writeText(measurers[index]->getOutput());
}

int DisplayController::getCurrentIndex()
{
    return index;
}