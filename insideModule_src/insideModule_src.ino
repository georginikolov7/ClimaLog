//*************************
//@Brief:
//- Receives data from outside module
//- Reads temp/hum inside
//- Display data on OLED display
//- Send data to Google Apps script

//@Cotroller: ESP32 WROOM D2

//*************************
#pragma GCC optimize("-fexceptions")  //enables exception handling

//Adding necessary libraries:
#include <EEPROM.h>  //used to store settings when power goes down (mounting height of outside module)
#include <DHT.h>
#include <Arduino.h>
#include "InsideMeasurer.h"
#include "OutsideMeasurer.h"
#include "RFRadio.h"
#include <SPI.h>
#include "Button.h"
#include "OLEDDisplay.h"
#include "DisplayController.h"
#include "ValueSelector.h"
#define USESERIAL  //to use serial monitor for debugging

#define EEPROM_SIZE 1  //we only need to store 1 byte - int mountingHeight
#define DEFAULT_MOUNTING_HEIGHT 100
#define rxPin 13
#define txPin 14
//Defining input pins:
#define INSIDE_DHT_TYPE DHT22
#define INSIDE_DHT_PIN 25
#define SET_BUTTON_PIN 33
#define DISPLAY_BUTTON_PIN 32

//display parameters:
#define I2C_DATA 26
#define I2C_CLK 27
#define WIDTH 128
#define HEIGHT 64
#define DISPLAY_ADDRESS 0x3c                //I2C address for OLED display
OLEDDisplay display(WIDTH, HEIGHT, &Wire);  //OLED display object

//create object instances:
DHT insideDHT(INSIDE_DHT_PIN, INSIDE_DHT_TYPE);  //dht22 temp/hum sensor for inside
RFRadio driver(2000, rxPin, txPin, 0);           //radio driver RH_ASK
Radio* radioPtr = &driver;
InsideMeasurer insideMeasurer(&insideDHT);  //create instance for measuring temp/humidity inside
OutsideMeasurer outsideMeasurer(radioPtr);  //create instance for receiving data from radio module (outside data)

//Button objects:
Button setButton(SET_BUTTON_PIN);
Button displayButton(DISPLAY_BUTTON_PIN);


//Measurers poiters array to pass to displayController:

Measurer* measurers[2] = {
  &insideMeasurer,
  &outsideMeasurer
};
Measurer* outsideMeasurers[1] = {
  &outsideMeasurer
};
//Display controller object, used for toggling display mode
DisplayController displayController(&display, measurers, 2);


unsigned long lastInsideReadTime = 0;
#define INSIDE_READ_INTERVAL 120000

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

  //EEPROM used to save mounting height
  EEPROM.begin(EEPROM_SIZE);

  //Set mounting height:
  int heightFromFlash = EEPROM.read(0);
  if (heightFromFlash == 255) {
    //height hasn't been set => set to default
    outsideMeasurer.setMountingHeight(DEFAULT_MOUNTING_HEIGHT);
  } else {
    outsideMeasurer.setMountingHeight(heightFromFlash);
  }
#ifdef USESERIAL
  Serial.printf("Mounting height: %i\n", outsideMeasurer.getMountingHeight());
#endif

  insideDHT.begin();
  delay(200);  //time to setup DHT
  insideMeasurer.readValues();

  //Initialize I2C comm and display object:
  Wire.begin(I2C_DATA, I2C_CLK, 100000);
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
#ifdef USESERIAL
    Serial.println(F("Processed data correctly"));
#endif
    if (outsideMeasurer.readValues()) {

      Serial.println();
      delay(5);
      displayController.displayData();
    }
  }
  if (setButton.isLongPressed()) {
    //Enter outsideMeasurer heightSetup:

    ValueSelector vs(outsideMeasurer.getMinHeight(), outsideMeasurer.getMaxHeight(), 5, &display, &setButton, "height", "cm");
    outsideMeasurer.setMountingHeight(vs.selectValue());
    displayController.displayData();
    Serial.println(outsideMeasurer.getMountingHeight());
  }
  if (millis() - lastInsideReadTime > INSIDE_READ_INTERVAL) {
    insideMeasurer.readValues();
    delay(5);
    displayController.displayData();
  }
  if (setButton.isShortPressed()) {
    displayController.changeDisplayMode();
    displayController.displayData();
    Serial.println(F("CHANGED MODE"));
    Serial.println(displayController.getIterator());
  }
}