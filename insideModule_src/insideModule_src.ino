//*************************
//@Brief:
//- Receives data from outside module
//- Reads temp/hum inside
//- Display data on OLED display
//- Send data to Google Apps script

//@Cotroller: ESP32 WROOM E2

//*************************

//Adding necessary libraries:
#include <DHT.h>
#include <Arduino.h>
#include "Measurer.h"
#include "InsideMeasurer.h"
#include "OutsideMeasurer.h"
#include <RH_ASK.h>
#include <SPI.h>
#include "Button.h"

#define USESERIAL

//Defining input pins:
#define INSIDE_DHT_TYPE DHT22
#define INSIDE_DHT_PIN 25
#define SET_BUTTON_PIN 33
#define DISPLAY_BUTTON_PIN 32

//create object instances:
DHT insideDHT(INSIDE_DHT_PIN, INSIDE_DHT_TYPE);
RH_ASK driver(2000, 12, 0, 0);
InsideMeasurer insideMeasurer(&insideDHT);
OutsideMeasurer outsideMeasurer(&driver);
Button setButton(SET_BUTTON_PIN);
Button displayButton(DISPLAY_BUTTON_PIN);

//Interrupts for the two buttons:
void IRAM_ATTR ISR_SET_BUTTON() {
  setButton.stateChanged();
}
void IRAM_ATTR ISR_DISPLAY_BUTTON() {
  displayButton.stateChanged();
}


void setup() {
  // put your setup code here, to run once:
  insideDHT.begin();
  Serial.begin(115200);  //start up the serial communication
  while (!Serial) {
  }
  if (!driver.init()) {
#ifdef USESERIAL
    Serial.println(F("COULD NOT INIT RADIO DRIVER!"));
#endif
  }


  attachInterrupt(SET_BUTTON_PIN, ISR_SET_BUTTON, CHANGE);
  attachInterrupt(DISPLAY_BUTTON_PIN, ISR_DISPLAY_BUTTON, CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (driver.available()) {
    if (outsideMeasurer.readValues()) {

      Serial.println(F("Processed data correctly"));
      Serial.println();
    }
  }
  if (setButton.isLongPressed()) {
    Serial.println(outsideMeasurer.getOutput());
    Serial.println();

    delay(500);
  }
}
