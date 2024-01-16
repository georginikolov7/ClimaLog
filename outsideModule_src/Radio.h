#include <stdint.h>
#ifndef Radio_h
#define Radio_h
#include <RH_ASK.h>
class Radio {
public:
  virtual bool send(uint32_t packet, uint8_t len);
  virtual bool receive(uint32_t packet, uint8_t len);
};
#endif