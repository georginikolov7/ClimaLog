
#ifndef Button_h
#define Button_h
#include "Arduino.h"

class Button {
public:
  Button(int pinNumber);
  void stateChanged();
  bool isLongPressed();
  bool isShortPressed();
  int getPinNumber();
  
private:
  int buttonPin;
  const int DEBOUNCE_DELAY = 20;
  const int LONGPRESS_DURATION = 1000;
  volatile unsigned long timeSinceLastDebounce = 0;
  volatile unsigned long lastPressTime = 0;
  volatile unsigned long lastReleaseTime = 0;
  volatile bool currentButtonState = 1;  //released by default   true - > currently pressed down false-> currently released
  volatile bool wasLongPressed = 1;
  volatile bool wasShortPressed = 1;  //true by default so that program does not count one short press upon setup (see isShortPressed function)
};

#endif