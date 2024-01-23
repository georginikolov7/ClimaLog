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

//Needed constants:
#define DHTPIN 2             //digital pin for Temp/Hum sensor
#define DHTSENSORTYPE DHT22  //type of ht sensor used

#define SNOWMETER_HIGH_ACCURACY
const int DISTANCE_OFFSET = -3;  //when measuring distance to snow, sensor tends to measure 3cm more than actual

// Initialize instances of all classes:
VL53L0X snowMeter;

DHT dht22(DHTPIN, DHTSENSORTYPE);

#define CE_PIN 10
#define CS_PIN 9
uint8_t readAddress[6] = "NODE1";
uint8_t writeAddress[6] = "NODE2";
RF24 radio;

//Battery level indicator:
#define VBAT_PIN A0
#define K_BATTERY 1.8  //y -> Voltage battery; x -> voltage GPIO
#define k_GPIO 0.005   //y- voltage at GPIO; x -> analog value
#define VBAT_MIN 6     //minimum volatage of battery
#define VBAT_MAX 8.4   //maximum voltage of battery
//Sensor values:
struct RadioPacket {
  float temperature = 0;  //measured temperature in celsius
  int humidity = 0;       //measured hum in %
  float distance = 0;     //measured distance in cm
  int batteryLevel = 0;   //sends current battery %
};
int packetsSent = 0;

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

  dht22.begin();  //begin the temp/hum sensor


  if (!radio.begin(CE_PIN, CS_PIN)) {
#ifdef USESERIAL
    throwErrorOnSerial("ERROR! COULD NOT INITIALIZE RF24 radio");
#endif
  }
  radio.setPayloadSize(sizeof(radioPacket));
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(writeAddress);
  radio.stopListening();
  radio.setAutoAck(false);
  radio.printPrettyDetails();  //for debugging

  //Set analog pin for VBAT as input:
  pinMode(A0, INPUT);
#ifdef USESERIAL
  Serial.println(F("Setup is complete"));
#endif
  delay(3000);  //setup delay
}

void loop() {
  // put your main code here, to run repeatedly:
  radio.powerUp();  //leave power saving mode of radio
  readSensorValues();
  readBatteryPercentage();
  radio.stopListening();                                         //set radio in Tx mode
  bool report = radio.write(&radioPacket, sizeof(radioPacket));  //sent package
  Serial.println(report);
  if (report) {
#ifdef USESERIAL
    Serial.println("Packet sent successfully!");
#endif
  }

  delay(200);         //delay before sleeping to prevent unpredictable behaviour
  radio.powerDown();  //enter power saving mode of radio
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
  radioPacket.temperature = dht22.readTemperature();
  radioPacket.humidity = dht22.readHumidity();
  radioPacket.distance = snowMeter.readRangeSingleMillimeters() / 10.00 + DISTANCE_OFFSET;  //read distance in mm, convert to cm and account for offset
#ifdef USESERIAL
  // Serial.print("Temperature: ");
  // Serial.print(radioPacket.temperature);
  // Serial.print(" Hum: ");
  // Serial.print(radioPacket.humidity);
  // Serial.print(" D: ");
  // Serial.println(radioPacket.distance);
  Serial.println("Values read and stored!");  //for debugging
#endif
}
void readBatteryPercentage() {
  float batteryVoltage = analogRead(VBAT_PIN) * K_BATTERY * k_GPIO;  //calculate actual voltage of battery using the coeficients for converting from the binary value to the GPIO voltage and then battery voltage
  int percentage = batteryVoltage / (VBAT_MAX - VBAT_MIN) * 100;     //calculate battery %
  radioPacket.batteryLevel = percentage;
}