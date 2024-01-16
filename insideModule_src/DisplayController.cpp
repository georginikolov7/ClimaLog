#include "DisplayController.h"
#include <Arduino.h>

DisplayController::DisplayController(Display* display, Measurer** measurers, int measurersCount) {
  this->display = display;
  this->measurers = measurers;
  this->measurersCount = measurersCount;
}
void DisplayController::changeDisplayMode() {
  iterator = (iterator + 1) % measurersCount;
  //increments the iterator by 1. Keeps in inside the bounds of measurers array
}
void DisplayController::displayData() {
  char output[50] = "\0";  //output string buffer
  if (measurers[iterator]->getLocation() == 1) {
    //selected module is inside
    strcat(output, "    IN\n");
  } else {
    //selected module is outside:
    int index = measurers[iterator]->getIndex();  //get the index:
    sprintf(output, "   OUT %i\n", index);
  }
  strcat(output, measurers[iterator]->getOutput());
  display->writeText(output);  //write the full output on the OLED
}
int DisplayController::getIterator() {
  return iterator;
}