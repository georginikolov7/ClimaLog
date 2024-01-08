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
#include <RH_ASK.h>    // include the RadioHead library for the 433 Mhz transmitter
#include <SPI.h>       //include dependant SPI library
#include <DHT.h>       //include dht library for temp/hum sensor
#include "Radio.h"

//Needed constants:
#define DHTPIN 2            //digital pin for Temp/Hum sensor
#define DHTSENSORTYPE DHT22  //type of ht sensor used
#define SNOWMETER_HIGH_ACCURACY
#define USESERIAL
#define SLEEPTIME_MILLISECONDS 300000  //define the sleep time of 5 minute(s)
const int DISTANCE_OFFSET = -3;        //when measuring distance to snow, sensor tends to measure 3cm more than actual

// Initialize instances of all classes:
VL53L0X snowMeter;

DHT dht22(DHTPIN, DHTSENSORTYPE);

RH_ASK driver;
Radio radio(&driver);

//Sensor values:
float temperature = 0;
float humidity = 0;
float distance = 0;

int packetsSent = 0;


void setup() {
  // put your setup code here, to run once:

#ifdef USESERIAL
  Serial.begin(9600);  //start serial monitor
  while (!Serial) {
    delay(1);
  }
#endif

  Wire.begin();
  radio.init();  //initialize the custom radio class
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

  dht22.begin();  //begin the temp/hum sensor

#ifdef USESERIAL
  Serial.println(F("Setup is complete"));
#endif
  delay(1000);  //setup delay
}

void loop() {
  // put your main code here, to run repeatedly:
  readSensorValues();
  appendValuesToRadio();
  bool isSent = radio.sendPacket();
  if (isSent) {
    packetsSent++;
  }
//PRINT ON SERIAL FOR DEBUGGING PURPOSES:
#ifdef USESERIAL
  uint8_t* buffer = radio.getPacket();
  Serial.print("Buffer: ");
  Serial.println((char*)buffer);
  Serial.print("Send state: ");
  Serial.println(isSent);
  Serial.print("Packets sent: ");
  Serial.println(packetsSent);
#endif
  delay(200);  //delay before sleeping to prevent unpredictable behaviour
  LowPower.longPowerDown(SLEEPTIME_MILLISECONDS);
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
void readSensorValues() {
  temperature = dht22.readTemperature();
  humidity = dht22.readHumidity();
  distance = snowMeter.readRangeSingleMillimeters() / 10.00 + DISTANCE_OFFSET;  //read distance in mm, convert to cm and account for offset
}

//@brief adds read values to radios buffer
void appendValuesToRadio() {
  radio.appendPacketFloat(temperature);
  radio.appendPacketFloat(humidity);
  radio.appendPacketFloat(distance);
}
