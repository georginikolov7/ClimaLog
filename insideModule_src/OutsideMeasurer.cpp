#include <stdio.h>
#include <string.h>
#include "OutsideMeasurer.h"
OutsideMeasurer::OutsideMeasurer(RF24 *radio, int index) {
  this->radio = radio;
  this->index = index;
  isInside = 0;
}
OutsideMeasurer::~OutsideMeasurer() {
  delete[] outsideOutput;
}

void OutsideMeasurer::readValues() {

  ReceiveBuffer buffer;
  radio->read(&buffer, sizeof(buffer));
  //Validate data:
  if (buffer.temperature > 50 || buffer.temperature < -30
      || buffer.humidity > 100 || buffer.humidity < 0
      || buffer.measuredDistance < 0) {
    //Invalid data
    return;
  }
  this->temperature = buffer.temperature;
  this->humidity = buffer.humidity;
  int snowDepth = mountingHeight - buffer.measuredDistance;
  snowDepth = ((snowDepth + 5 / 2) / 5) * 5;  //round snowDepth to closest multiple of 5
  this->snowDepth = snowDepth;
  this->batteryLevel = buffer.batteryLevel;
}
const char *OutsideMeasurer::getOutput() {
  const int SNOW_STRING_LEN = 16;
  const int OUTSIDE_OUTPUT_LEN = SNOW_STRING_LEN + OUTPUT_BUFFER_SIZE;
  delete[] outsideOutput;
  outsideOutput = new char[OUTSIDE_OUTPUT_LEN];
  outsideOutput[0] = '\0';
  int availableSpace = OUTSIDE_OUTPUT_LEN - strlen(outsideOutput) - 1;
  strncpy(outsideOutput, Measurer::getOutput(), availableSpace);
  char snowString[SNOW_STRING_LEN] = "\0";
  availableSpace = SNOW_STRING_LEN - 1;
  snprintf(snowString, availableSpace, "\nSnow %i cm", snowDepth);
  availableSpace = OUTSIDE_OUTPUT_LEN - strlen(outsideOutput) - 1;
  strncat(outsideOutput, snowString, availableSpace);
  return outsideOutput;
}
int OutsideMeasurer::getSnowDepth() {
  return snowDepth;
}
void OutsideMeasurer::setMountingHeight(int newHeight) {
  mountingHeight = newHeight;
  //Save value to flash:
  EEPROM.write(index - 1, mountingHeight);
  EEPROM.commit();
}