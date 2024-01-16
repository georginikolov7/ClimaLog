#include <stdint.h>
#ifndef Radio_h
#define Radio_h
class Radio {
public:
  virtual bool send(uint8_t* message) = 0;
  virtual bool receive(uint8_t* receiveBuffer, uint8_t* len) = 0;
  virtual ~Radio() {}
};
#endif