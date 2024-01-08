#include "InsideMeasurer.h"
#include <DHT.h>
InsideMeasurer::InsideMeasurer(DHT* dhtSensor) {
  this->dhtSensor = dhtSensor;
  isInside = 1;
};
InsideMeasurer::~InsideMeasurer() {
}
bool InsideMeasurer::readValues() {
  temperature = dhtSensor->readTemperature();
  humidity = dhtSensor->readHumidity();
  return true;
}