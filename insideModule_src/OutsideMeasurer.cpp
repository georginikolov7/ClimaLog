#include "OutsideMeasurer.h"
OutsideMeasurer::OutsideMeasurer(RH_ASK *radioDriver, int mountingDistance) {
  this->mountingDistance = mountingDistance;
  this->radioDriver = radioDriver;
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
    snowDepth = mountingDistance - measuredDistance;
    snowDepth = ((snowDepth + 5 / 2) / 5) * 5;  //round snowDepth to multiples of 5
  } catch (...) {
    //Received buffer's format is incorrect => dump that packet

    return false;
  }
  return true;
}
const char *OutsideMeasurer::getOutput() {
  char *buffer = new char[50];
  strcpy(buffer, Measurer::getOutput());
  char snowString[5];
  sprintf(snowString, "\nSnow: %i cm", snowDepth);
  strcat(buffer, snowString);
  return buffer;
}
float OutsideMeasurer::getTemperature() {
  return Measurer::getTemperature();
}
float OutsideMeasurer::getHumidity() {
  return Measurer::getHumidity();
}
int OutsideMeasurer::getSnowDepth() {
  return snowDepth;
}