#include "DisplayController.h"
#include <Arduino.h>

DisplayController::DisplayController(Display* display, InsideMeasurer* insideMeasurer, OutsideMeasurer** outsideMeasurers, int outsideMeasurersCount) {
  this->display = display;
  const int measurersCount = outsideMeasurersCount + 1;
  measurers = new Measurer*[measurersCount];
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
  char output[64] = "\0";  //output string buffer
  if (measurers[iterator]->isInsideModule() == 1) {
    //selected module is inside
    strcat(output, "    IN\n");
  } else {
    //selected module is outside:
    int index = measurers[iterator]->getIndex();  //get the index:
    sprintf(output, "   OUT %i\n", index);

    if (outsideMeasurers[iterator - 1]->batLevelIsLow()) {
      //size of outsideMeasurers array is measurers size - 1 => index is iterator - 1
      //Draw the bitmap on the display:
    }
  }
  strcat(output, measurers[iterator]->getOutput());
  display->resetDisplay();
  display->writeText(output);  //write the full output on the OLED
}
int DisplayController::getIterator() {
  return iterator;
}