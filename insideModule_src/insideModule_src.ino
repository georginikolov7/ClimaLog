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

#define DEFAULT_MOUNTING_HEIGHT 100
#define rxPin 13  //SPI receive
#define txPin 14  //SPI send

#define INSIDE_DHT_TYPE DHT22
#define INSIDE_DHT_PIN 25  //DHT22 on pin 25
#define SET_BUTTON_PIN 33
#define DISPLAY_BUTTON_PIN 32

//display parameters:
#define I2C_DATA 26                         //pin for data
#define I2C_CLK 27                          //pin for clock
#define WIDTH 128                           //width of display in px
#define HEIGHT 64                           //height of display i px
#define DISPLAY_ADDRESS 0x3c                //I2C address for OLED display
OLEDDisplay display(WIDTH, HEIGHT, &Wire);  //OLED display object

//Button objects:
Button setButton(SET_BUTTON_PIN);
Button displayButton(DISPLAY_BUTTON_PIN);

DHT insideDHT(INSIDE_DHT_PIN, INSIDE_DHT_TYPE);  //dht22 temp/hum sensor for inside
RFRadio driver(2000, rxPin, txPin, 0);           //radio driver RH_ASK

#define OUTSIDE_MODULES_COUNT 1
InsideMeasurer insideMeasurer(&insideDHT);    //create instance for measuring temp/humidity inside
OutsideMeasurer outsideMeasurer(&driver, 1);  //create instance for receiving data from radio module (outside data)

//Measurers poiters array to pass to displayController:
Measurer* measurers[2] = {
  &insideMeasurer,
  &outsideMeasurer
};
OutsideMeasurer* outsideMeasurers[1] = {
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
  EEPROM.begin(OUTSIDE_MODULES_COUNT);  //we need to keep one int value for every outside module

  //Set mounting heights on each outside module:
  for (int i = 0; i < OUTSIDE_MODULES_COUNT; i++) {
    int heightFromFlash = EEPROM.read(i);
    if (heightFromFlash == 255) {
      //height hasn't been set yet:
      outsideMeasurers[i]->setMountingHeight(DEFAULT_MOUNTING_HEIGHT);
    } else {
      //set height from flash:
      outsideMeasurers[i]->setMountingHeight(heightFromFlash);
    }
  }

  insideDHT.begin();  //initialize the inside DHT
  delay(200);         //time to setup DHT
  insideMeasurer.readValues();

  //Initialize I2C comm and display object:
  Wire.begin(I2C_DATA, I2C_CLK, 100000);
  display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS);
  //default setup for display:
  display.setupDisplay();
  displayController.displayData();

  if (!driver.init()) {
#ifdef USESERIAL
    Serial.println(F("COULD NOT INIT RADIO DRIVER!"));
#endif
  }
  //Attach interrupts for buttons:
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
    //Enter height setup:
    heightSetup();
  }

  if (millis() - lastInsideReadTime > INSIDE_READ_INTERVAL) {
    //read inside values:
    insideMeasurer.readValues();
    delay(5);
    displayController.displayData();  //if inside module is showing on display => update values
  }

  if (setButton.isShortPressed()) {
    displayController.changeDisplayMode();
    displayController.displayData();
    Serial.println(F("CHANGED MODE"));
    Serial.println(displayController.getIterator());
  }
}

void heightSetup() {
  ValueSelector outsideModuleSelector(1, OUTSIDE_MODULES_COUNT, 1, &display, &setButton, "module");  //create selector instance to select the index of the outside module we want to change the height of
  int moduleIndex = outsideModuleSelector.selectValue() - 1;                                         //index is -1, because it starts from 0
  //Create value selector to select height:
  ValueSelector vs(outsideMeasurers[moduleIndex]->getMinHeight(), outsideMeasurers[moduleIndex]->getMaxHeight(), 5, &display, &setButton, "height", "cm");
  outsideMeasurers[moduleIndex]->setMountingHeight(vs.selectValue());  //set the new height
  displayController.displayData();                                     //display the standary display output
#ifdef USESERIAL
  Serial.printf("New height: %i", outsideMeasurers[moduleIndex]->getMountingHeight());  //for debugging only
#endif
}