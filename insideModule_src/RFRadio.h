#ifndef RFRadio_h
#define RFRadio_h
#include <RH_ASK.h>
#include "Radio.h"
class RFRadio : public Radio, public RH_ASK {
public:
  RFRadio(uint16_t speed = 2000, uint8_t rxPin = 11, uint8_t txPin = 12, uint8_t pttPin = 10, bool pttInverted = false)
    : RH_ASK(speed, rxPin, txPin, pttPin, pttInverted) {}
  bool send(uint8_t* message) override;
  bool receive(uint8_t* buffer, uint8_t* len) override;
  ~RFRadio() {}
};
#endif