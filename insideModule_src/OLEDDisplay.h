#ifndef OLEDDisplay_h
#define OLEDDisplay_h
#include "Display.h"
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

class OLEDDisplay : public Display, public Adafruit_SSD1306 {
public:
  OLEDDisplay(uint8_t w, uint8_t h, TwoWire* twi = &Wire, int8_t rst_pin = -1, uint32_t clkDuring = 400000UL, uint32_t clkAfter = 100000UL)
    : Adafruit_SSD1306(w, h, &Wire, rst_pin, clkDuring, clkAfter){};
  ~OLEDDisplay() {}
  void writeText(const char* text) override;
};

#endif