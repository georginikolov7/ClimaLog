//****************************************************

//Controller - Arduino NANO
//@brief Outside module source code.
//Reads temp/humidity/distanceToSnow
//Sends them via 433 mhz radio to inside (main module)
//Goes into deep sleep for a given time period
//****************************************************

#include <Wire.h>              //used for I2C communication with distance sensor
#include <RH_ASK.h>            // include the RadioHead library for the 433 Mhz transmitter
#include <SPI.h>               //include dependant SPI library
#include <Adafruit_VL53L0X.h>  //include ToF distance sensor library
#include <DHT.h>               //include dht library for temp/hum sensor

#define DHTPin 4             //digital pin for Temp/Hum sensor
#define DHTSensorType DHT22  //type of ht sensor used
#define RadioPin 12          //digital pin for Radio

// Initialize instances of all classes:
Adafruit_VL53L0X snowMeter = Adafruit_VL53L0X();
DHT dht22(DHTPin, DHTSensorType);
RH_ASK radio;


//Define variable that will be used when measuring and transmitting:
float temperature = 0;
float humidity = 0;
float measuredDistance = 0;
const float DISTANCE_OFFSET = -30;  //when measuring distance to snow sensor tends to measure 3cm more than actual
void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);  //start serial monitor
  while (!Serial) {
    delay(1);
  }

  if (!setupSnowMeter()) {
    ThrowErrorOnSerial("ERROR OCCURED!!! COULD NOT INITIALIZE ToF DISTANCE SENSOR");
  }

  dht22.begin();

  if (!radio.init()) {
    ThrowErrorOnSerial("ERROR OCCURED!!! COULD NOT INITIALIZE RF 433MHZ RADIO");
  }

  Serial.println("Setup is complete");
  delay(200);   //setup delay
  Serial.println(dht22.readTemperature());
  Serial.println(dht22.readHumidity());
}

void loop() {
  // put your main code here, to run repeatedly:
  
}


//@brief setup sensor and set accuracy to high
//@returns true if successful, otherwise false
bool setupSnowMeter() {
  if (!snowMeter.begin()) {
    return false;
  }
  if (!snowMeter.configSensor(snowMeter.VL53L0X_SENSE_HIGH_ACCURACY)) {
    return false;
  }
  return true;
}
//@brief error handling function for debugging purposes
//@params String errorMessage
void ThrowErrorOnSerial(String message) {
  Serial.println(message);
  while (true) {  //waits and does nothing
    delay(1);
  }
}