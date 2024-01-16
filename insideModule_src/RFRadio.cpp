#include <stdint.h>
#include "RFRadio.h"
bool RFRadio::send(uint8_t* message) {
  uint8_t len = sizeof(message);
  bool isSent = 0;
  isSent = RH_ASK::send(message, len);
  RH_ASK::waitPacketSent();
  return isSent;
}
bool RFRadio::receive(uint8_t* buffer, uint8_t* len) {
  if (RH_ASK::recv(buffer, len)) {
    int i;
    return true;
  }
  return false;
}