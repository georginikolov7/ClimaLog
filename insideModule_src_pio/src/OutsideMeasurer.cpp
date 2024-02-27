#include "OutsideMeasurer.h"
#include <stdio.h>
#include <string.h>

OutsideMeasurer::OutsideMeasurer(const char* name, int indexOfModule, RF24* radio, bool isBatteryPowered)
    : Measurer(name)
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
    if (buffer.temperature == INCORRECT_MEASURE_VALUE) {
        strncpy(temperature, "noVal", sizeof(temperature));
    } else {
        // Save temperature as string
        dtostrf(buffer.temperature, 4, 2, this->temperature);
    }

    if (buffer.humidity == INCORRECT_MEASURE_VALUE) {
        strncpy(humidity, "noVal", sizeof(humidity));
    } else {

        // Save humidity as strings:
        itoa(buffer.humidity, this->humidity, 10);
    }
    if (buffer.measuredDistance == INCORRECT_MEASURE_VALUE) {
        strncpy(this->snowDepth, "noVal", sizeof(this->snowDepth));
    } else {
        // Save snowDepth as string:
        int snowDepth = mountingHeight - buffer.measuredDistance;
        // round snowDepth to closest multiple of 5:
        snowDepth = ((snowDepth + 5 / 2) / 5) * 5;
        itoa(snowDepth, this->snowDepth, 10);
    }
    this->batteryLevel = buffer.batteryLevel;
}

const char* OutsideMeasurer::getOutput()
{
    // Call base get output (to append standart output)
    Measurer::getOutput();

    // Add snow data:
    const int SNOW_STRING_LEN = 16;
    char snowString[SNOW_STRING_LEN] = "\0";
    snprintf(snowString, SNOW_STRING_LEN - strlen(snowString), "\nSnow %s cm", snowDepth);

    strncat(output, snowString, OUTPUT_BUFFER_SIZE - strlen(output));
    return output;
}

const char* OutsideMeasurer::getSnowDepth()
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