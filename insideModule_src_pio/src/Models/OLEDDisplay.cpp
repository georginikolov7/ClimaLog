#include "OLEDDisplay.h"
#include <stdint.h>

void OLEDDisplay::writeText(const char* text)
{
    Adafruit_SSD1306::setCursor(0, 0);
    Adafruit_SSD1306::println(text);
    Adafruit_SSD1306::display();
}
void OLEDDisplay::writeAt(int row, const char* text)
{
    Adafruit_SSD1306::setCursor(0, row * textSize * DEFAULT_TEXT_HEIGTH);
    Adafruit_SSD1306::println(text);
    Adafruit_SSD1306::display();
}
void OLEDDisplay::resetDisplay()
{
    Adafruit_SSD1306::clearDisplay();
    // Adafruit_SSD1306::display();
}
void OLEDDisplay::drawBitmap(int x, int y, const uint8_t* bitmap, int w, int h)
{
    Adafruit_SSD1306::drawBitmap(x, y, bitmap, w, h, WHITE);
    Adafruit_SSD1306::display();
}
void OLEDDisplay::defaultSetup()
{
    Adafruit_SSD1306::clearDisplay();
    Adafruit_SSD1306::setTextColor(WHITE);
    Adafruit_SSD1306::setTextSize(2);
    textSize = 2;
    Adafruit_SSD1306::setCursor(0, 0);
}