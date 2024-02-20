#include <Arduino.h>
#include "InsideMeasurer.h"
#include <DHT.h>
InsideMeasurer::InsideMeasurer(const char *name, DHT *dhtSensor) : Measurer(name)
{
  this->dhtSensor = dhtSensor;
};
InsideMeasurer::~InsideMeasurer()
{
}
void InsideMeasurer::readValues()
{
  //To do data validation + retrier
  temperature = dhtSensor->readTemperature();
  float readHum = dhtSensor->readHumidity();
  // if (isnan(readHum)) {
  //   readHum = 0;
  // }
  // if (isnan(temperature)) {
  //   temperature = 0;
  // }
  humidity = round(readHum);
}