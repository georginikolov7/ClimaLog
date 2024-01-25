#include "Measurer.h"

const char* Measurer::getOutput() {
  delete[] output;  //clear output buffer incase it is not empty
  output = new char[OUTPUT_BUFFER_SIZE];

  char tempString[7];  //4 digits + . + +/- + \0 delimiter total of 7 chars
  dtostrf(temperature, 4, 2, tempString);
  sprintf(output, "T %s C\nHum %i %%", tempString, humidity);
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
