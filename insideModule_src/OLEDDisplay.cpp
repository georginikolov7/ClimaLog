#include "OLEDDisplay.h"

void OLEDDisplay::writeText(const char* text) {
  Adafruit_SSD1306::clearDisplay();
  Adafruit_SSD1306::setCursor(0, 0);
  Adafruit_SSD1306::println(text);
  Adafruit_SSD1306::display();
}
void OLEDDisplay::writeAt(int row, const char* text) {
  Adafruit_SSD1306::setCursor(0, row);
  Adafruit_SSD1306::write(text);
  Adafruit_SSD1306::display();
}
void OLEDDisplay::resetDisplay() {
  Adafruit_SSD1306::clearDisplay();
  Adafruit_SSD1306::display();
}
