#include "InsideMeasurer.h"
#include <DHT.h>
InsideMeasurer::InsideMeasurer(DHT* dhtSensor) {
  this->dhtSensor = dhtSensor;
};
bool InsideMeasurer::readValues() {
  temperature = dhtSensor->readTemperature();
  humidity = dhtSensor->readHumidity();
  return true;
}

const char* InsideMeasurer::getOutput(){
  return Measurer::getOutput();
}
float InsideMeasurer::getTemperature(){
  return Measurer::getTemperature();
}
float InsideMeasurer::getHumidity(){
  return Measurer::getHumidity();
}