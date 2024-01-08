//*************************
//@Brief:
//- Receives data from outside module
//- Reads temp/hum inside
//- Display data on OLED display
//- Send data to Google Apps script

//@Cotroller: ESP32 devkitC

//*************************
//#pragma GCC optimize("-fexceptions")  //enables exception handling

//Adding necessary libraries:
#include <EEPROM.h>  //used to store settings when power goes down (mounting height of outside module)
#include <DHT.h>
#include <Arduino.h>
#include "InsideMeasurer.h"
#include "OutsideMeasurer.h"
#include <RH_ASK.h>
#include <SPI.h>
#include "Button.h"
#include "OLEDDisplay.h"
#include "DisplayController.h"

#define USESERIAL  //to use serial monitor for debugging

#define EEPROM_SIZE 1  //we only need to store 1 byte - int mountingHeight
#define DEFAULT_MOUNTING_HEIGHT 100
//Defining input pins:
#define INSIDE_DHT_TYPE DHT22
#define INSIDE_DHT_PIN 25
#define SET_BUTTON_PIN 33
#define DISPLAY_BUTTON_PIN 32

//display parameters:
#define WIDTH 128
#define HEIGHT 64
#define DISPLAY_ADDRESS 0x3c                //I2C address for OLED display
OLEDDisplay display(WIDTH, HEIGHT, &Wire);  //OLED display object

//create object instances:
DHT insideDHT(INSIDE_DHT_PIN, INSIDE_DHT_TYPE);  //dht22 temp/hum sensor for inside
RH_ASK driver(1000, 12, 14, 0);                   //radio driver RH_ASK
InsideMeasurer insideMeasurer(&insideDHT);  //create instance for measuring temp/humidity inside
OutsideMeasurer outsideMeasurer(&driver);   //create instance for receiving data from radio module (outside data)

//Button objects:
Button setButton(SET_BUTTON_PIN);
Button displayButton(DISPLAY_BUTTON_PIN);


//Measurers poiters array to pass to displayController:

Measurer* measurers[2] = {
  &insideMeasurer,
  &outsideMeasurer
};
//Display controller object, used for toggling display mode
DisplayController displayController(&display, measurers, 2);


unsigned long lastInsideReadTime = 0;

//Interrupts for the two buttons:
void IRAM_ATTR ISR_SET_BUTTON() {
  setButton.stateChanged();
}
void IRAM_ATTR ISR_DISPLAY_BUTTON() {
  displayButton.stateChanged();
}


void setup() {
  // put your setup code here, to run once:
#ifdef USESERIAL
  Serial.begin(115200);  //start up the serial communication
  while (!Serial) {
  }
#endif

  //Save default mounting height to flash and set it to outsideMeasurer object
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(0, 100);
  outsideMeasurer.setMountingHeight(EEPROM.read(0));

  insideDHT.begin();

  insideMeasurer.readValues();
  //Initialize I2C comm and display object:
  Wire.begin(26, 27, 100000);
  display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  delay(50);
  display.clearDisplay();
  displayController.displayData();

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
      displayController.displayData();
    }
  }
  if (millis() - lastInsideReadTime > 120000) {
    insideMeasurer.readValues();
    displayController.displayData();
  }
  if (setButton.isShortPressed()) {

    Serial.println(measurers[0]->getOutput());
    Serial.println();

    displayController.changeDisplayMode();
    displayController.displayData();
    Serial.println(F("CHANGED MODE"));
    Serial.println(displayController.getIterator());
    delay(500);
  }
}