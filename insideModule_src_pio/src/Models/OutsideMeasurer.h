#include <stdint.h>
#ifndef OutsideMeasurer_h
#define OutsideMeasurer_h
#include <EEPROM.h>
#include <RF24.h>
#include <nRF24L01.h>

#include "Models/Button.h"
#include "Models/Measurer.h"

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
struct ReceiveBuffer {
    int32_t humidity;
    int32_t batteryLevel;
    float temperature;
    float measuredDistance;
};

class OutsideMeasurer : public Measurer {
public:
    OutsideMeasurer()
        : Measurer()
    {
    }
    OutsideMeasurer(const char* name, int index, RF24* radio,
        bool isPluggedIn = false);
    ~OutsideMeasurer();
    // OutsideMeasurer& operator=(const OutsideMeasurer&);
    bool operator==(OutsideMeasurer& other);
    void setMountingHeight(int newHeight);
    const char* getOutput() override;
    int getMountingHeight() { return mountingHeight; }
    int getMinHeight() { return MIN_HEIGHT; }
    int getMaxHeight() { return MAX_HEIGHT; }
    const char* getBatteryLevel();
    bool batLevelIsLow();
    const char* getPowerSource();
    void readValues() override;

    const char* getSnowDepth();

private:
    RF24* radio;
    int index=0;
    bool isPluggedIn;
    char snowDepth[10];
    char batteryStatus[15];
    int batteryLevel = 0; // outside module battery level in %
    int mountingHeight = 0; // set mounting height in cm
    char* powerSource;
    const static int MAX_HEIGHT = 110; // maximum mounitng height in cm
    const static int MIN_HEIGHT = 50; // minimal mounting height in cm
    const static int LOW_BATTERY_PERCENTAGE = 30;
};
#endif