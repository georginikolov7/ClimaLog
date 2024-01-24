#include "Server.h"
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

  // try {
  //   char *token;
  //   char *rest = (char *)receiveBuffer;
  //   token = strtok_r(rest, ";", &rest);
  //   temperature = atof(token);
  //   token = strtok_r(rest, ";", &rest);
  //   humidity = atof(token);
  //   token = strtok_r(rest, ";", &rest);
  //   measuredDistance = atof(token);
  //   snowDepth = mountingHeight - measuredDistance;

  //   snowDepth = ((snowDepth + 5 / 2) / 5) * 5;  //round snowDepth to closest multiple of 5
  // } catch (...) {
  //   //Received buffer's format is incorrect => dump that packet
  //   Serial.println("EBASI MAMATA");
  //   return false;
}
const char *OutsideMeasurer::getOutput() {
  delete[] outsideOutput;
  outsideOutput = new char[OUTPUT_BUFFER_SIZE];
  strcpy(outsideOutput, Measurer::getOutput());
  char snowString[64] = "\0";
  sprintf(snowString, "\nSnow %i cm", snowDepth);
  strcat(outsideOutput, snowString);
  return outsideOutput;
}
int OutsideMeasurer::getSnowDepth() {
  return snowDepth;
}
void OutsideMeasurer::setMountingHeight(int newHeight) {
  mountingHeight = newHeight;
  //Save value to flash:
  EEPROM.write(0, mountingHeight);
  EEPROM.commit();
}