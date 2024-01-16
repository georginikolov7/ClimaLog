#include "Measurer.h"

const char* Measurer::getOutput() {
  delete[] output;  //clear output buffer incase it is not empty
  output = new char[OUTPUT_BUFFER_SIZE];

  char tempString[7];  //4 digits + . + +/- + \0 delimiter total of 7 chars
  dtostrf(temperature, 4, 2, tempString);
  char humString[3];  //2 digits + \0 delimiter total of 3 chars
  dtostrf(humidity, 2, 0, humString);

  sprintf(output, "T %s C\nHum %s %%", tempString, humString);
  return output;
}
float Measurer::getTemperature() {
  return temperature;
}
float Measurer::getHumidity() {
  return humidity;
}
bool Measurer::getLocation() {
  return isInside;
}
int Measurer::getIndex() {
  return index;
}
Measurer::~Measurer() {
  delete[] output;
}
