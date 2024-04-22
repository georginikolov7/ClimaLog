#include "InsideMeasurer.h"
#include <Arduino.h>
#include <DHT.h>
InsideMeasurer::InsideMeasurer(const char* name, DHT* dhtSensor)
    : Measurer(name)
{
    this->dhtSensor = dhtSensor;
};
InsideMeasurer::~InsideMeasurer()
{
}
bool InsideMeasurer::validateTemperature(float& temperature)
{
    return temperature > -35 && temperature < 50;
}
bool InsideMeasurer::validateHumidity(int& humidity)
{
    return humidity > 0 && humidity <= 100;
}
void InsideMeasurer::readValues()
{
    float tempFloat = 0;
    tempFloat = dhtSensor->readTemperature();
    if (!validateTemperature(tempFloat)) {
        int retryCount = 0;
        while (!validateTemperature(tempFloat)) {
            retryCount++;
            tempFloat = dhtSensor->readTemperature();
            if (retryCount == MAX_RETRY_COUNT) {
                strncpy(temperature, "noValue", sizeof(temperature));
                break;
            }
        }
    } else {
        dtostrf(tempFloat, 4, 2, temperature);
    }

    int humInt = (int)dhtSensor->readHumidity();
    if (!validateHumidity(humInt)) {
        int retryCount = 0;
        while (!validateHumidity(humInt)) {
            retryCount++;
            humInt = dhtSensor->readHumidity();
            if (retryCount == MAX_RETRY_COUNT) {
                strncpy(humidity, "noValue", sizeof(humidity));
                break;
            }
        }
    } else {
        itoa(humInt, humidity, 10);
    }
}