#include <stdio.h>
#include <string.h>
#include "DisplayController.h"
#include <Arduino.h>
#include "Icons.h"
#include "WiFi.h"

DisplayController::DisplayController(Display *display, InsideMeasurer *insideMeasurer, OutsideMeasurer **outsideMeasurers, int outsideMeasurersCount)
{
  this->display = display;
  const int measurersCount = outsideMeasurersCount + 1;
  this->outsideMeasurers = outsideMeasurers;
  this->measurers = new Measurer *[measurersCount];
  this->measurersCount = measurersCount;
  // 1st element is the insideMeasurer:
  measurers[0] = insideMeasurer;

  // Fill rest of array with outsideMeasurers:
  for (int i = 0; i < outsideMeasurersCount; i++)
  {
    measurers[i + 1] = outsideMeasurers[i];
  }
}
DisplayController::~DisplayController()
{
  delete[] measurers;
}

void DisplayController::changeDisplayMode()
{
  index = (index + 1) % measurersCount;
  // increments the iterator by 1. Keeps it inside the bounds of measurers array
}

void DisplayController::displayData()
{
  display->resetDisplay();
  if (index >= 1 && outsideMeasurers[index - 1]->batLevelIsLow())
  {
    // size of outsideMeasurers array is measurers size - 1 => index is iterator - 1

    // Draw the bitmap on the display:
    int xPosition = display->getWidth() - BATTERY_INDICATOR_WIDTH;
    display->drawBitmap(xPosition, 0, batteryIndicator, BATTERY_INDICATOR_WIDTH, BATTERY_INDICATOR_HEIGHT);
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    // Display WiFi icon below battery icon
    display->drawBitmap(display->getWidth() - NO_WIFI_WIDTH, display->getHeight() - BATTERY_INDICATOR_HEIGHT, epd_bitmap_no_wifi, NO_WIFI_WIDTH, NO_WIFI_HEIGHT);
  }
  display->writeText(measurers[index]->getOutput()); // write the full output on the OLED
}

int DisplayController::getCurrentIndex()
{
  return index;
}