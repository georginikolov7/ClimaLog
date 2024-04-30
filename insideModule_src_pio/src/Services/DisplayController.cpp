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

void DisplayController::begin(InsideMeasurer* insideMeasurer, OutsideMeasurersRepo* outsideMeasurers)
{
    this->insideMeasurer = insideMeasurer;
    this->outsideMeasurers = outsideMeasurers;
    ready = true;
}
DisplayController::~DisplayController()
{
}

void DisplayController::changeDisplayMode()
{
    if (!ready) {
        Serial.println("Display controller not initialized!");
        return;
    }

    index = (index + 1) % (outsideMeasurers->getCount() + 1);
    // increments the iterator by 1. Keeps it inside the bounds of measurers array
}

void DisplayController::displayData()
{
    //Serial.printf("Current index: %i\r\n", index);
    //Serial.printf("Current count: %i\r\n", outsideMeasurers->getCount());
    if (!ready || index >= outsideMeasurers->getCount() + 1) {
        return;
    }
    display->resetDisplay();
    if (index == 0) {
        display->writeText(insideMeasurer->getOutput());
    } else {

        display->writeText((*outsideMeasurers)[index - 1].getOutput());
        if ((*outsideMeasurers)[index - 1].batLevelIsLow()) {
            // size of outsideMeasurers array is measurers size - 1 => index is iterator - 1
            // Draw the bitmap on the display:
            int xPosition = DISPLAY_WIDTH - BATTERY_INDICATOR_WIDTH;
            display->drawBitmap(xPosition, 0, batteryIndicator, BATTERY_INDICATOR_WIDTH, BATTERY_INDICATOR_HEIGHT);
        }
    }
}

int DisplayController::getCurrentIndex()
{
    return index;
}