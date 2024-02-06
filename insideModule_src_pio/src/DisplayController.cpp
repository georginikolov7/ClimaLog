#include <stdio.h>
#include <string.h>
#include "DisplayController.h"
#include <Arduino.h>

DisplayController::DisplayController(Display* display, InsideMeasurer* insideMeasurer, OutsideMeasurer** outsideMeasurers, int outsideMeasurersCount) {
  this->display = display;
  const int measurersCount = outsideMeasurersCount + 1;
  this->outsideMeasurers = outsideMeasurers;
  this->measurers = new Measurer*[measurersCount];
  this->measurersCount = measurersCount;
  //1st element is the insideMeasurer:
  measurers[0] = insideMeasurer;

  //Fill rest of array with outsideMeasurers:
  for (int i = 0; i < outsideMeasurersCount; i++) {
    measurers[i + 1] = outsideMeasurers[i];
  }
}
DisplayController::~DisplayController() {
  delete[] measurers;
}

void DisplayController::changeDisplayMode() {
  iterator = (iterator + 1) % measurersCount;
  //increments the iterator by 1. Keeps it inside the bounds of measurers array
}

void DisplayController::displayData() {

  const int OUTPUT_BUFFER_SIZE = 254;
  char output[OUTPUT_BUFFER_SIZE] = "\0";  //output string buffer
  int availableSpace = OUTPUT_BUFFER_SIZE - 1;
  display->resetDisplay();

  if (measurers[iterator]->isInsideModule() == 1) {
    //selected module is inside
    strncat(output, "IN\n", availableSpace);
  } else {
    //selected module is outside:
    int index = measurers[iterator]->getIndex();  //get the index (which outside module is this?):
    availableSpace = OUTPUT_BUFFER_SIZE - strlen(output) - 1;
    snprintf(output, availableSpace, "OUT %i\n", index);

    if (outsideMeasurers[iterator - 1]->batLevelIsLow()) {
      //size of outsideMeasurers array is measurers size - 1 => index is iterator - 1
      //Draw the bitmap on the display:
      int xPosition = display->getWidth() - BATTERY_INDICATOR_WIDTH;
      display->drawBitmap(xPosition, 0, batteryIndicator, BATTERY_INDICATOR_WIDTH, BATTERY_INDICATOR_HEIGHT);
    }
  }
  availableSpace = OUTPUT_BUFFER_SIZE - strlen(output) - 1;
  strncat(output, measurers[iterator]->getOutput(), availableSpace);
  display->writeText(output);  //write the full output on the OLED
}
int DisplayController::getIterator() {
  return iterator;
}