#include "SPI.h"
#include "Arduino.h"
#include "Radio.h"

Radio::Radio(RH_ASK* driver)
  : driver(driver) {}

void Radio::init() {
  if (!driver->init()) {
    Serial.println(F("ERROR! Could not initialize radio driver"));
  }
  Serial.println("Yes");
}
bool Radio::appendPacketFloat(float value) {
  if (charsCount <= sizeof(packet) / sizeof(packet[0]) - BUFFER_SIZE) {  //checks whether packet is full
    char buffer[BUFFER_SIZE];
    dtostrf(value, 4, 2, buffer);
    strcat(packet, buffer);
    strcat(packet, ";");
    charsCount += BUFFER_SIZE;
    return true;
  }
  return false;
}

bool Radio::sendPacket() {
  if (driver->send((uint8_t*)packet, strlen(packet))) {
    driver->waitPacketSent();
    charsCount = 0;
    return true;
  }
  return false;
}
uint8_t* Radio::getPacket() {
  return (uint8_t*)packet;
}