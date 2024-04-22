#ifndef IDisplay_h
#define IDisplay_h
#include <Arduino.h>
class IDisplay {
public:
    virtual void writeText(const char* text) = 0;
    virtual void writeAt(int row, const char* text) = 0;
    virtual void drawBitmap(int x, int y, const uint8_t* bitmap, int w, int h) = 0;
    virtual int getWidth() = 0;
    virtual int getHeight() = 0;
    virtual void resetDisplay() = 0;
    virtual void defaultSetup() = 0;
    virtual ~IDisplay() { }
};
#endif