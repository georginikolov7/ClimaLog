#include "Measurer.h"

const char* Measurer::getOutput() {
  delete[] output;  //clear output buffer
  output = new char[OUTPUT_BUFFER_SIZE];

  char tempString[10] = "\0";  //4 digits + . + +/- + \0 delimiter total of 7 chars
  dtostrf(temperature, 4, 2, tempString);
  int availableSpace = OUTPUT_BUFFER_SIZE-1;
  snprintf(output, availableSpace, "T %s C\nHum %i %%", tempString, humidity);
  return output;
}
float Measurer::getTemperature() {
  return temperature;
}
int Measurer::getHumidity() {
  return humidity;
}
bool Measurer::isInsideModule() {
  return isInside;
}
int Measurer::getIndex() {
  return index;
}
Measurer::~Measurer() {
  delete[] output;
}