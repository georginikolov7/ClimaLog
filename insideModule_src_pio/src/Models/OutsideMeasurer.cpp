#include "OutsideMeasurer.h"

#include <stdio.h>
#include <string.h>

OutsideMeasurer::OutsideMeasurer(const char* name, int index,
    RF24* radio, bool isPluggedIn)
    : Measurer(name)
{
    this->radio = radio;
    this->isPluggedIn = isPluggedIn;
    powerSource = new char[11];
    if (isPluggedIn) {
        batteryLevel = 100;
        strncpy(powerSource, "Plugged in", strlen(powerSource));
    } else {
        strncpy(powerSource, "Battery", strlen(powerSource));
    }

    this->index = index;
}
OutsideMeasurer::~OutsideMeasurer()
{
    delete[] powerSource;
}

bool OutsideMeasurer::operator==(OutsideMeasurer& other)
{
    // If names are equal than the objects are equal
    return other.getName() == this->name;
}
const char* OutsideMeasurer::getPowerSource()
{
    return powerSource;
}
bool OutsideMeasurer::batLevelIsLow()
{
    if (isPluggedIn) {
        return false;
    }

    return batteryLevel <= LOW_BATTERY_PERCENTAGE; // returns true if battery is at capacity below 30%
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
        Serial.println(buffer.measuredDistance);
        // round snowDepth to closest multiple of 5:
        snowDepth = ((snowDepth + 5 / 2) / 5) * 5;
        itoa(snowDepth, this->snowDepth, 10);
    }

    if (!isPluggedIn) {
        this->batteryLevel = buffer.batteryLevel;
    }
}

const char* OutsideMeasurer::getOutput()
{
    // Call base get output (to append standart output)
    Measurer::getOutput();

    // Add snow data:
    const int SNOW_STRING_LEN = 16;
    char snowString[SNOW_STRING_LEN] = "\0";
    snprintf(snowString, SNOW_STRING_LEN - strlen(snowString), "\nSnow %s cm",
        snowDepth);

    strncat(output, snowString, OUTPUT_BUFFER_SIZE - strlen(output));
    return output;
}
const char* OutsideMeasurer::getBatteryLevel()
{
    if (isPluggedIn) {
        strncpy(batteryStatus, "100", sizeof(batteryStatus) - 1);
    } else {
        itoa(batteryLevel, batteryStatus, 10);
    }
    return batteryStatus;
}
const char* OutsideMeasurer::getSnowDepth()
{
    return snowDepth;
}
void OutsideMeasurer::setMountingHeight(int newHeight)
{
    mountingHeight = newHeight;
    // Save value to flash:
    EEPROM.write(index, mountingHeight);
    EEPROM.commit();
}
