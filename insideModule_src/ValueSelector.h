#ifndef ValueSelector_h
#define ValueSelector_h

#include "Display.h"
#include "Button.h"

class ValueSelector {
public:
  ValueSelector(int minValue, int maxValue, int incrementValue, Display* display, Button* setButton, const char* keyword, const char* measureUnit);
  ~ValueSelector();
  int selectValue();

private:
  const int BLINK_INTERVAL = 300;
  int minValue;
  int maxValue;
  int incrementValue;
  
  Display* display;
  Button* setButton;

  char* measureUnit;
  char* keyword;
};
#endif