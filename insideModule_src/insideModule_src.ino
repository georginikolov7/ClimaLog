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
#include <Arduino.h>
#include <EEPROM.h>  //used to store settings when power goes down (mounting height of outside module)
#include <DHT.h>
#include <SPI.h>  //used for communicating with nRF24 radio
#include <RF24.h>
#include <nRF24L01.h>
#include <printf.h>  //for printing rf24 details (debugging)
#include "InsideMeasurer.h"
#include "OutsideMeasurer.h"
#include "Button.h"
#include "OLEDDisplay.h"
#include "DisplayController.h"
#include "ValueSelector.h"

#define USESERIAL  // uncomment to use serial monitor for debugging


#define INSIDE_DHT_TYPE DHT22
#define INSIDE_DHT_PIN 4  //DHT22 pin

//display pins and dimensions:
#define I2C_DATA 21                         //I2C pin for data
#define I2C_CLK 22                          //I2C pin for clock
#define WIDTH 128                           //width of display in px
#define HEIGHT 64                           //height of display in px
#define DISPLAY_ADDRESS 0x3c                //I2C address for OLED display
OLEDDisplay display(WIDTH, HEIGHT, &Wire);  //OLED display object

//Button objects:
#define SET_BUTTON_PIN 2
#define DISPLAY_BUTTON_PIN 32
Button setButton(SET_BUTTON_PIN);
Button displayButton(DISPLAY_BUTTON_PIN);

DHT insideDHT(INSIDE_DHT_PIN, INSIDE_DHT_TYPE);  //dht22 temp/hum sensor for inside

//Radio:
#define CE_PIN 17                          //SPI chip enable
#define CS_PIN 5                           //SPI chip select pin
uint8_t readAddresses[][6] = { "OUTM1" };  //addresses for outside modules
RF24 radio(CE_PIN, CS_PIN);

#define DEFAULT_MOUNTING_HEIGHT 100  //modules work best when mounted at 1 meter (but can be changed using the buttons)
#define OUTSIDE_MODULES_COUNT 1
InsideMeasurer insideMeasurer(&insideDHT);   //create instance for measuring temp/humidity inside
OutsideMeasurer outsideMeasurer(&radio, 1);  //create instance for receiving data from radio module (outside data)


//OutsideMeasurer pointers
OutsideMeasurer* outsideMeasurers[OUTSIDE_MODULES_COUNT] = {
  &outsideMeasurer
};

//Display controller object, used for toggling display mode
DisplayController displayController(&display, &insideMeasurer, outsideMeasurers, 1);


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
  // SPISettings spiSettings(2000000, MSBFIRST, SPI_MODE0);  //lower SPI frequency to 4MHZ so it works with nrf24
  // SPI.beginTransaction(spiSettings);
  // Serial.println("HELLO?");
  SPI.begin();
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


  //Attach interrupts for buttons:
  attachInterrupt(SET_BUTTON_PIN, ISR_SET_BUTTON, CHANGE);
  attachInterrupt(DISPLAY_BUTTON_PIN, ISR_DISPLAY_BUTTON, CHANGE);

  if (!radio.begin()) {
#ifdef USESERIAL
    Serial.println(F("Could not initialize RF24 module!"));
    while (true) {
    }
#endif
  }
  setupRadio();

#ifdef USESERIAL
  Serial.println(F("Setup is complete!"));
#endif
}

uint8_t receivePipe;

void loop() {
  // put your main code here, to run repeatedly:
  if (radio.available(&receivePipe)) {
    //receivePipe corresponds to index of outsideMeasurer + 1
    Serial.println("Test");
    outsideMeasurers[receivePipe - 1]->readValues();
#ifdef USESERIAL
    Serial.printf("Received data from outside module %i\n", receivePipe);  //for debuging
    Serial.println(outsideMeasurer.getOutput());
    Serial.println(outsideMeasurer.getBatteryLevel());
#endif
    displayController.displayData();  //update values on display
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


void setupRadio() {
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setPayloadSize(sizeof(ReceiveBuffer));
  for (int i = 1; i <= OUTSIDE_MODULES_COUNT; i++) {
    radio.openReadingPipe(i, readAddresses[i - 1]);  //open a reading pipe for each of the connected outside modules
  }
#ifdef USESERIAL
  radio.printPrettyDetails();  //for debugging
#endif
  radio.startListening();
}