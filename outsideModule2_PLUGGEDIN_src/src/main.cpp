

//****************************************************
// Controller - Arduino NANO
//@brief Outside module source code.
// Reads temp/humidity/distanceToSnow
// Sends them via 433 mhz radio to inside (main module)
// Goes into deep sleep for a given time period (to preserve battery life)
//****************************************************

#include "VL53L0X.h"  //include ToF distance sensor library
#include <DHT.h>      //include dht library for temp/hum sensor
#include <LowPower.h> //include sleep library
#include <RF24.h>
#include <SPI.h>  //include dependant SPI library
#include <Wire.h> //used for I2C communication with distance sensor
#include <nRF24L01.h>

#include <printf.h> //for printing rf24 details (debugging)
#define USESERIAL
#define SLEEPTIME_MILLISECONDS 10000

// If value is equal to -42 => measure is not correct(used in data validation
// and displaying in inside)
const int INCORRECT_MEASURE_VALUE = -42;

// Data validation:
#define MAX_MOUNTING_HEIGHT 200 // sensor measures to 2 meters max
const int MEASURES_COUNT = 5;
const int MAX_MEASURE_RETRIES = 3; // measures up to 3 incorrect values

// DHT22 object
#define DHTPIN 2            // digital pin for Temp/Hum sensor
#define DHTSENSORTYPE DHT22 // type of ht sensor used
DHT dht22(DHTPIN, DHTSENSORTYPE);

// Distance sensor:
#define SNOWMETER_HIGH_ACCURACY
// #define SNOWMETER_HIGH_RANGE
const int OFFSET = -2.5; // after 50 cm sensor measures 5 cm short
VL53L0X snowMeter;

// Radio object:
#define CE_PIN 10
#define CS_PIN 9
const uint8_t writeAddress[6] =
    "2OUTM"; // Change adress according to index of the current  module
RF24 radio(CE_PIN, CS_PIN);

// Sensor values struct:
struct RadioPacket {
  int32_t humidity;     // measured hum in %
  int32_t batteryLevel; // sends current battery %
  float temperature;    // measured temperature in celsius
  float distance;       // measured distance in cm
};
RadioPacket radioPacket; // declare struct for radioData

// @brief error handling function for debugging purposes
// @params c-string errorMessage
void throwErrorOnSerial(const char *message);

// Data validators:
bool validateTemperature(float &temperature) {
  return temperature > -35 && temperature < 50;
}
bool validateHumidity(int32_t &humidity) {
  return humidity > 0 && humidity <= 100;
}
bool validateDistance(float &distance) {
  return distance >= 0 && distance < MAX_MOUNTING_HEIGHT;
}
void measureTemperature(float &temperature);

void measureDistance(float &distance);

void measureHumidity(int32_t &humidity);

//@brief reads and stores sensor values: temperature/humidity/distance
void readSensorValues(RadioPacket *radioPacket);

//_________________________________________________________________________________________________________
void setup() {
#ifdef USESERIAL
  printf_begin();
  Serial.begin(9600); // start serial monitor
  while (!Serial) {
    delay(1);
  }
#endif

  // setup distance sensor:
  Wire.begin(); // start I2C comms
  snowMeter.setTimeout(500);
  if (!snowMeter.init()) {
#ifdef USESERIAL
    throwErrorOnSerial("ERROR! COULD NOT INIT ToF SENSOR!");
#endif
  }

#ifdef SNOWMETER_HIGH_ACCURACY
  // increase timing budget to 200 ms
  snowMeter.setMeasurementTimingBudget(200000);
#endif

#ifdef SNOWMETER_HIGH_RANGE
  // lower the return signal rate limit (default is 0.25 MCPS)
  snowMeter.setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  snowMeter.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  snowMeter.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
#endif

  dht22.begin(); // begin the temp/hum sensor

  // setup radio:
  SPI.begin();
  if (!radio.begin()) {
#ifdef USESERIAL
    throwErrorOnSerial("ERROR! COULD NOT INITIALIZE RF24 radio");
#endif
  }
  radio.setPayloadSize(sizeof(RadioPacket));
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(writeAddress);
  radio.printPrettyDetails(); // for debugging

#ifdef USESERIAL
  Serial.println(F("Setup is complete"));
#endif
  delay(500); // setup delay
}

void loop() {
  radio.powerUp(); // leave power saving mode of radio

  delay(500); // time to bootup radio

  readSensorValues(&radioPacket);

  radio.stopListening(); // set radio in Tx mode
  if (radio.write(&radioPacket, sizeof(radioPacket))) {
    // sent package
#ifdef USESERIAL
    Serial.println("Sent data");
#endif
  }
  delay(200);
  radio.powerDown(); // enter power saving mode of radio
  LowPower.longPowerDown(SLEEPTIME_MILLISECONDS); // set arduino to sleep
}

void throwErrorOnSerial(const char *message) {
  Serial.println((char *)message);
  while (true) { // waits and does nothing
    delay(1);
  }
}
void measureTemperature(float &temperature) {

  temperature = dht22.readTemperature();
  if (!validateTemperature(temperature)) {
    int retryCount = 0;
    while (!validateTemperature(temperature)) {
      retryCount++;
      temperature = dht22.readTemperature();
      if (retryCount == MAX_MEASURE_RETRIES) {
        temperature = INCORRECT_MEASURE_VALUE;
        break;
      }
    }
  }
}
void measureHumidity(int32_t &humidity) {
  humidity = (int)dht22.readHumidity();
  if (!validateHumidity(humidity)) {
    int retryCount = 0;
    while (!validateHumidity(humidity)) {
      retryCount++;
      humidity = dht22.readTemperature();
      if (retryCount == MAX_MEASURE_RETRIES) {
        humidity = INCORRECT_MEASURE_VALUE;
        break;
      }
    }
  }
}
void measureDistance(float &distance) {
  float distanceSum = 0;
  int incorrectValuesCount = 0;
  for (int i = 0; i < MEASURES_COUNT; i++) {
    float currentDistance = snowMeter.readRangeSingleMillimeters() /
                            10.00; // read distance in mm, convert to cm
    Serial.println(currentDistance);
    if (!validateDistance(currentDistance)) {
      // incorrect values...
      incorrectValuesCount++;
      i--; // reduce incrementor by 1 to retake measure
      if (incorrectValuesCount > MAX_MEASURE_RETRIES) {
        distance = (float)INCORRECT_MEASURE_VALUE;
        return;
      }
    } else {
      distanceSum += currentDistance;
    }
  }
  distance = distanceSum / MEASURES_COUNT + OFFSET;
}
void readSensorValues(RadioPacket *radioPacket) {

  measureTemperature(radioPacket->temperature);
  // Serial.println("Gasi1");
  measureHumidity(radioPacket->humidity);
  // Serial.println("Gasi2");
  measureDistance(radioPacket->distance);
  // Serial.println("Gasi");

#ifdef USESERIAL
  Serial.print("Temperature: ");
  Serial.println(radioPacket->temperature);
  Serial.print(" Hum: ");
  Serial.println(radioPacket->humidity);
  Serial.print(" Distance: ");
  Serial.println(radioPacket->distance);
  Serial.println("Values read and stored!"); // for debugging
#endif
}