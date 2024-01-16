#include "OutsideMeasurer.h"
OutsideMeasurer::OutsideMeasurer(Radio *radio, int index) {
  this->radio = radio;
  this->index = index;
  isInside = 0;
}
OutsideMeasurer::~OutsideMeasurer() {
  delete[] outsideOutput;
}

bool OutsideMeasurer::readValues() {
  float measuredDistance = 0;

  uint8_t receiveBuffer[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(receiveBuffer);

  radio->receive(receiveBuffer, &len);
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

    snowDepth = ((snowDepth + 5 / 2) / 5) * 5;  //round snowDepth to closest multiple of 5
  } catch (...) {
    //Received buffer's format is incorrect => dump that packet
    Serial.println("EBASI MAMATA");
    return false;
  }
  return true;
}
const char *OutsideMeasurer::getOutput() {
  delete[] outsideOutput;
  outsideOutput = new char[OUTSIDE_OUTPUT_BUFFER_SIZE];
  strcpy(outsideOutput, Measurer::getOutput());
  char snowString[20] = "\0";
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