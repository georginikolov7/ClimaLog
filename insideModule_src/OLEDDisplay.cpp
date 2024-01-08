#include "OLEDDisplay.h"

void OLEDDisplay::writeText(const char* text) {
  Adafruit_SSD1306::clearDisplay();
  Adafruit_SSD1306::setCursor(0, 0);
  Adafruit_SSD1306::println(text);
  Adafruit_SSD1306::display();
}