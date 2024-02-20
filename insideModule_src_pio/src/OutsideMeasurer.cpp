#include <stdio.h>
#include <string.h>
#include "OutsideMeasurer.h"

OutsideMeasurer::OutsideMeasurer(const char *name, int indexOfModule, RF24 *radio, bool isBatteryPowered) : Measurer(name)
{
  // INDEXATION STARTS FROM 0!!!
  this->radio = radio;
  this->isBatteryPowered = isBatteryPowered;
  this->indexOfModule = indexOfModule;
}
OutsideMeasurer::~OutsideMeasurer()
{
}

void OutsideMeasurer::readValues()
{

  ReceiveBuffer buffer;
  radio->read(&buffer, sizeof(buffer));

  // Validate data:
  if (buffer.temperature > 50 || buffer.temperature < -30 || buffer.humidity > 100 || buffer.humidity < 0 || buffer.measuredDistance < 0 || buffer.measuredDistance > 200)
  {
    // Invalid data
    return;
  }

  this->temperature = buffer.temperature;
  this->humidity = buffer.humidity;
  int snowDepth = mountingHeight - buffer.measuredDistance;

  // round snowDepth to closest multiple of 5:
  snowDepth = ((snowDepth + 5 / 2) / 5) * 5;

  this->snowDepth = snowDepth;
  this->batteryLevel = buffer.batteryLevel;
}

const char *OutsideMeasurer::getOutput()
{
  // Call base get output (to append standart output)
  Measurer::getOutput();

  // Add snow data:
  const int SNOW_STRING_LEN = 16;
  char snowString[SNOW_STRING_LEN] = "\0";
  snprintf(snowString, SNOW_STRING_LEN - strlen(snowString), "\nSnow %i cm", snowDepth);

  strncat(output, snowString, OUTPUT_BUFFER_SIZE - strlen(output));
  return output;
}

int OutsideMeasurer::getSnowDepth()
{
  return snowDepth;
}
void OutsideMeasurer::setMountingHeight(int newHeight)
{
  mountingHeight = newHeight;
  // Save value to flash:
  EEPROM.write(indexOfModule, mountingHeight);
  EEPROM.commit();
}