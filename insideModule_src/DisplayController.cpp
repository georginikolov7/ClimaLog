#include "DisplayController.h"
#include <Arduino.h>

DisplayController::DisplayController(Display* display, Measurer** measurers, int measurersCount) {
  this->display = display;
  this->measurers = measurers;
  this->measurersCount = measurersCount;
}
void DisplayController::changeDisplayMode() {
  iterator = (iterator + 1) % measurersCount;
}
void DisplayController::displayData() {
  char output[50] = "\0";
  if (measurers[iterator]->getLocation() == 1) {
    strcat(output, "    IN\n");
  } else {
    strcat(output, "   OUT\n");
  }
  strcat(output, measurers[iterator]->getOutput());
  display->writeText(output);
}
int DisplayController::getIterator() {
  return iterator;
}