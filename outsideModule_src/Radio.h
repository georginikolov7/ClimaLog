#ifndef Radio_h
#define Radio_h
#include <RH_ASK.h>
class Radio {
public:
  Radio(RH_ASK* driver);
  void init();
  bool appendPacketFloat(float value);
  bool sendPacket();
  uint8_t* getPacket();
private:
  RH_ASK* driver;
  char packet[RH_ASK_MAX_MESSAGE_LEN] = "\0";
  const int BUFFER_SIZE = 7;  //buffer used for converting float values to strings
  int charsCount = 7;
};
#endif