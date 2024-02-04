//****************************************************

//Controller - Arduino NANO
//@brief Outside module source code.
//Reads temp/humidity/distanceToSnow
//Sends them via 433 mhz radio to inside (main module)
//Goes into deep sleep for a given time period (to preserve battery life)
//****************************************************

#include <LowPower.h>  //include sleep library
#include "VL53L0X.h"   //include ToF distance sensor library
#include <Wire.h>      //used for I2C communication with distance sensor
#include <DHT.h>       //include dht library for temp/hum sensor
#include <SPI.h>       //include dependant SPI library
#include <RF24.h>
#include <nRF24L01.h>
#include <printf.h>  //for printing rf24 details (debugging)
#define USESERIAL
#define SLEEPTIME_MILLISECONDS 300000  //define the sleep time of 5 minute(s)

//Data validation:
#define MAX_MOUNTING_HEIGHT 150
#define MEASURE_RETRY_COUNT 3
bool measureIsSuccessful = false;  //boolean flag for checking whether measured values are realistic
//DHT22 object
#define DHTPIN 2             //digital pin for Temp/Hum sensor
#define DHTSENSORTYPE DHT22  //type of ht sensor used
DHT dht22(DHTPIN, DHTSENSORTYPE);

//Distance sensor:
#define SNOWMETER_HIGH_ACCURACY
//#define SNOWMETER_HIGH_RANGE

const int OFFSET = -2.5;  //after 50 cm sensor measures 5 cm short

VL53L0X snowMeter;

//Radio object:
#define CE_PIN 10
#define CS_PIN 9
const uint8_t writeAddress[6] = "1OUTM";  //Change adress according to index of the current  module
RF24 radio;

//Battery level indicator:
#define VBAT_PIN A0
#define K_BATTERY 1.8  //y -> Voltage battery; x -> voltage GPIO
#define k_GPIO 0.005   //y- voltage at GPIO; x -> analog value
#define VBAT_MIN 6     //minimum volatage of battery
#define VBAT_MAX 8.4   //maximum voltage of battery

//Sensor values struct:
struct RadioPacket {
  int32_t humidity;      //measured hum in %
  int32_t batteryLevel;  //sends current battery %
  float temperature;     //measured temperature in celsius
  float distance;        //measured distance in cm
};
RadioPacket radioPacket;

void setup() {
  // put your setup code here, to run once:

#ifdef USESERIAL
  printf_begin();      //printf used by nrf24
  Serial.begin(9600);  //start serial monitor
  while (!Serial) {
    delay(1);
  }
#endif
  //setup distance sensor:
  Wire.begin();  //start I2C comms
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

#if defined SNOWMETER_HIGH_RANGE
  // lower the return signal rate limit (default is 0.25 MCPS)
  snowMeter.setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  snowMeter.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  snowMeter.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
#endif

  dht22.begin();  //begin the temp/hum sensor

  //setup radio:
  if (!radio.begin(CE_PIN, CS_PIN)) {
#ifdef USESERIAL
    throwErrorOnSerial("ERROR! COULD NOT INITIALIZE RF24 radio");
#endif
  }
  radio.setPayloadSize(sizeof(RadioPacket));
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(writeAddress);
  radio.printPrettyDetails();  //for debugging

  //Set analog pin for VBAT as input:
  pinMode(A0, INPUT);
#ifdef USESERIAL
  Serial.println(F("Setup is complete"));
#endif
  delay(500);  //setup delay
}

void loop() {
  // put your main code here, to run repeatedly:
  radio.powerUp();  //leave power saving mode of radio
  delay(200);       //time to bootup radio
  readBatteryPercentage();
  if (readSensorValues()) {
    radio.stopListening();                           //set radio in Tx mode
    radio.write(&radioPacket, sizeof(radioPacket));  //sent package
  }

  delay(200);                                      //delay before sleeping to prevent unpredictable behaviour
  radio.powerDown();                               //enter power saving mode of radio
  LowPower.longPowerDown(SLEEPTIME_MILLISECONDS);  //set arduino to sleep
}



// @brief error handling function for debugging purposes
// @params String errorMessage
void throwErrorOnSerial(const char* message) {
  Serial.println((char*)message);
  while (true) {  //waits and does nothing
    delay(1);
  }
}

//@brief reads and stores sensor values: temperature/humidity/distance
bool readSensorValues() {
  for (int i = 0; i < MEASURE_RETRY_COUNT; i++) {
    radioPacket.temperature = dht22.readTemperature();
    radioPacket.humidity = dht22.readHumidity();
    radioPacket.distance = snowMeter.readRangeSingleMillimeters() / 10.00;  //read distance in mm, convert to cm and account for offset
    if (radioPacket.temperature > 45
        || radioPacket.temperature < -35
        || radioPacket.humidity > 100
        || radioPacket.humidity <= 0
        || radioPacket.distance < 0
        || radioPacket.distance > MAX_MOUNTING_HEIGHT) {
      //Inalid data-> retry
    } else {
      radioPacket.distance += OFFSET;

#ifdef USESERIAL
      Serial.print("Temperature: ");
      Serial.print(radioPacket.temperature);
      Serial.print(" Hum: ");
      Serial.print(radioPacket.humidity);
      Serial.print(" Distance: ");
      Serial.println(radioPacket.distance);
      Serial.println("Values read and stored!");  //for debugging
#endif
      return true;
    }
  }
  return false;
}
void readBatteryPercentage() {
  int readValue = analogRead(VBAT_PIN);
  Serial.println(readValue);
  float batteryVoltage = readValue * K_BATTERY * k_GPIO;                       //calculate actual voltage of battery using the coeficients for converting from the binary value to the GPIO voltage and then battery voltage
  int percentage = (batteryVoltage - VBAT_MIN) / (VBAT_MAX - VBAT_MIN) * 100;  //calculate battery %
  radioPacket.batteryLevel = percentage;
}