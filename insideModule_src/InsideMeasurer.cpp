#include <Arduino.h>
#include "InsideMeasurer.h"
#include <DHT.h>
InsideMeasurer::InsideMeasurer(DHT* dhtSensor) {
  this->dhtSensor = dhtSensor;
  isInside = 1;
};
InsideMeasurer::~InsideMeasurer() {
}
void InsideMeasurer::readValues() {
  temperature = dhtSensor->readTemperature();
  float readHum = dhtSensor->readHumidity();
  if (isnan(readHum)) {
    readHum = 0;
  }
  if (isnan(temperature)) {
    temperature = 0;
  }
  humidity = round(readHum);
}