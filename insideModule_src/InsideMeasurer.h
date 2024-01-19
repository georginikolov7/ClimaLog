#ifndef InsideMeasurer_h
#define InsideMeasurer_h
#include "Measurer.h"
#include <DHT.h>

class InsideMeasurer : public Measurer {
public:
  InsideMeasurer(DHT* dhtSensor);
  ~InsideMeasurer();
  void readValues() override;
private:
  DHT* dhtSensor;
};

#endif