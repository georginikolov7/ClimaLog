#ifndef InsideMeasurer_h
#define InsideMeasurer_h
#include "Measurer.h"
#include <DHT.h>

class InsideMeasurer : public Measurer {
public:
    InsideMeasurer(const char* name, DHT* dhtSensor);
    ~InsideMeasurer();
    void readValues() override;

private:
    bool validateTemperature(float& temperature);
    bool validateHumidity(int& humidity);
    DHT* dhtSensor;
    const int MEASURES_COUNT=5;
    const int MAX_RETRY_COUNT=3;
};

#endif