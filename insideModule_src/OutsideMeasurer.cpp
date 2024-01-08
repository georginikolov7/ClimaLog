#include "OutsideMeasurer.h"
OutsideMeasurer::OutsideMeasurer(RH_ASK *radioDriver) {
  this->radioDriver = radioDriver;
  isInside = 0;
}
OutsideMeasurer::~OutsideMeasurer() {
  delete[] outsideOutput;
}
bool OutsideMeasurer::readValues() {
  float measuredDistance = 0;
  uint8_t receiveBuffer[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t bufflen = sizeof(receiveBuffer);
  radioDriver->recv(receiveBuffer, &bufflen);
  Serial.println((char *)receiveBuffer);  //for debugging
  try {
    char *token;
    char *rest = (char *)receiveBuffer;
    token = strtok_r(rest, ";", &rest);
    temperature = atof(token);
    token = strtok_r(rest, ";", &rest);
    humidity = atof(token);
    token = strtok_r(rest, ";", &rest);
    measuredDistance = atof(token);
    snowDepth = mountingHeight - measuredDistance;
    snowDepth = ((snowDepth + 5 / 2) / 5) * 5;  //round snowDepth to multiples of 5
  } catch (...) {
    //Received buffer's format is incorrect => dump that packet
    Serial.println("EBASI MAMATA");
    return false;
  }
  return true;
}
const char *OutsideMeasurer::getOutput() {
  delete[] outsideOutput;
  outsideOutput = new char[OUTPUT_BUFFER_SIZE + 10];
  strcpy(outsideOutput, Measurer::getOutput());
  char snowString[5];
  sprintf(snowString, "\nSnow: %i cm", snowDepth);
  strcat(outsideOutput, snowString);
  return outsideOutput;
}
int OutsideMeasurer::getSnowDepth() {
  return snowDepth;
}
void OutsideMeasurer::setMountingHeight(int newHeight) {
  mountingHeight = newHeight;
  EEPROM.write(0, mountingHeight);
}