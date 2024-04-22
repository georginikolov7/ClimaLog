#ifndef OLEDDisplay_h
#define OLEDDisplay_h
#include "IDisplay.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class OLEDDisplay : public IDisplay, public Adafruit_SSD1306 {
public:
    OLEDDisplay(uint8_t w, uint8_t h, TwoWire* twi = &Wire, int8_t rst_pin = -1, uint32_t clkDuring = 400000UL, uint32_t clkAfter = 100000UL)
        : Adafruit_SSD1306(w, h, &Wire, rst_pin, clkDuring, clkAfter) {};
    ~OLEDDisplay() { }
    void defaultSetup() override;
    void writeText(const char* text) override;
    void writeAt(int row, const char* text) override;
    void drawBitmap(int x, int y, const uint8_t* bitmap, int w, int h) override;
    void resetDisplay() override;
    void setupDisplay();
    int getWidth()
    {
        return 128;
    }
    int getHeight()
    {
        return 64;
    }

private:
    const int DEFAULT_TEXT_HEIGTH = 18;
    const int DEFAULT_TEXT_WIDTH = 16;
    int textSize = 1;
};

#endif