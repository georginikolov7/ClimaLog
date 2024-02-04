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

//For sending the data to the web API:
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#define USESERIAL  // uncomment to use serial monitor for debugging

//Wifi:
//array for holding wifi networks (even index->ssid, odd index-> pass )
#define SAVED_NETWORKS_COUNT 2
const int WIFI_CONNECT_TIME_LIMIT = 60000;  //1 minute for connecting
const int NETWORKS_ARRAY_SIZE = SAVED_NETWORKS_COUNT * 2;
const char savedNetworks[][50] = {
  "kartala", "kartala1333", "AndroidAPe456", "12345678"
};
const int httpsPort = 443;
const char* hostID = "AKfycbxsifh2gFosBuIHMA6PFGe74tT1oHAJoEsMCbjFBev2n1fIuR0mgsFi8s6Jw5M5Rbmm";  //the id of the google apps script
//WiFiClientSecure client;
HTTPClient https;
//Obtaining the time:
int lastHour = 0;
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7200;   //UTC +2 hours id +7200 seconds
const int daylightOffset_sec = 0;  //not summer time
int currentHour = 0;
bool HTTPS_sent = false;
#define INSIDE_DHT_TYPE DHT22
#define INSIDE_DHT_PIN 13  //DHT22 pin

//display pins and dimensions:
#define I2C_DATA 21                         //I2C pin for data
#define I2C_CLK 22                          //I2C pin for clock
#define WIDTH 128                           //width of display in px
#define HEIGHT 64                           //height of display in px
#define DISPLAY_ADDRESS 0x3c                //I2C address for OLED display
OLEDDisplay display(WIDTH, HEIGHT, &Wire);  //OLED display object

//Button objects:
#define SET_BUTTON_PIN 33
#define DISPLAY_BUTTON_PIN 32
Button setButton(SET_BUTTON_PIN);
Button displayButton(DISPLAY_BUTTON_PIN);

DHT insideDHT(INSIDE_DHT_PIN, INSIDE_DHT_TYPE);  //dht22 temp/hum sensor for inside

#define OUTSIDE_MODULES_COUNT 1
//Radio:
#define CE_PIN 17                                               //SPI chip enable
#define CS_PIN 5                                                //SPI chip select pin
uint8_t readAddresses[OUTSIDE_MODULES_COUNT][6] = { "1OUTM" };  //addresses for outside modules
uint8_t receivePipe;                                            //used to store the pipe num where data was received
RF24 radio(CE_PIN, CS_PIN);

#define DEFAULT_MOUNTING_HEIGHT 100          //modules work best when mounted at 1 meter (but can be changed using the buttons)
InsideMeasurer insideMeasurer(&insideDHT);   //create instance for measuring temp/humidity inside
OutsideMeasurer outsideMeasurer(&radio, 1);  //create instance for receiving data from radio module (outside data)
//OutsideMeasurer out2(&radio, 2);

//OutsideMeasurer pointers
OutsideMeasurer* outsideMeasurers[OUTSIDE_MODULES_COUNT] = {
  &outsideMeasurer
};

//Display controller object, used for toggling display mode
DisplayController displayController(&display, &insideMeasurer, outsideMeasurers, OUTSIDE_MODULES_COUNT);


const int HTTP_SEND_INTERVAL = 1;  //send request once every 4 hours
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


  //Attach interrupts for buttons:
  attachInterrupt(SET_BUTTON_PIN, ISR_SET_BUTTON, CHANGE);
  attachInterrupt(DISPLAY_BUTTON_PIN, ISR_DISPLAY_BUTTON, CHANGE);

  //Initialize I2C comm and display object:
  Wire.begin(I2C_DATA, I2C_CLK, 100000);
  display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS);
  //default setup for display:
  display.setupDisplay();
  display.resetDisplay();
  display.writeText("Booting...");
  delay(700);

  //Establish Wifi connection:
  selectWifiNetwork();
  delay(500);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  //Obtain hour for HTTPS sender:
  if (!getCurrentHour()) {
#ifdef USESERIAL
    Serial.println("Failed to obtain time from NTC");
#endif
  } else {
#ifdef USESERIAL
    Serial.printf("Hour: %i\n", currentHour);
#endif
    lastHour = currentHour;
  }

  SPI.begin();

  //EEPROM used to save mounting height
  EEPROM.begin(OUTSIDE_MODULES_COUNT);  //we need to keep one int value(height) for every outside module

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
  delay(500);         //time to setup DHT
  insideMeasurer.readValues();

  displayController.displayData();  //displays first measurer in array (inside measurer by default)


  //initialize RF24 radio:
  if (!radio.begin()) {
#ifdef USESERIAL
    Serial.println(F("Could not initialize RF24 module!"));
    while (true) {
    }
#endif
  }
  setupRadio();

//Debugging info:
#ifdef USESERIAL
  Serial.println(F("Setup is complete!"));
  Serial.println(F("RF24 settings:"));
  radio.printPrettyDetails();
  Serial.print(F("Wifi IP Adress: "));
  Serial.println(WiFi.localIP());
#endif
}


void loop() {
  String readCommand;
  if (Serial.available() > 0) {
    readCommand = Serial.readStringUntil('\n');
  }
  //Check if data is received:
  if (radio.available(&receivePipe)) {
    //receivePipe corresponds to index of outsideMeasurer + 1
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

  //change displayed data if button is pressed:
  if (displayButton.isShortPressed()) {
    displayController.changeDisplayMode();
    displayController.displayData();
#ifdef USESERIAL
    Serial.println(F("CHANGED MODE"));
    Serial.println(displayController.getIterator());
#endif
  }

  //Read inside values if time has passed:
  if (millis() - lastInsideReadTime > INSIDE_READ_INTERVAL) {
    //read inside values:
    insideMeasurer.readValues();
    displayController.displayData();  //if inside module is showing on display => update values
    lastInsideReadTime = millis();
  }
  if (WiFi.status() == WL_CONNECTED && getCurrentHour()) {
    // Serial.printf("Current hour: %i\n", currentHour);
    // Serial.printf("Last hour: %i\n", lastHour);
    if (currentHour - lastHour == HTTP_SEND_INTERVAL) {
      HTTPS_sent = false;
      lastHour = currentHour;
    }
    //Send https request once every 4 hours:
    if (!HTTPS_sent || readCommand == "HTTPS") {
      //build the https url:
      HTTPS_sent = true;
      const char* url = buildHTTPSURL();
#ifdef USESERIAL
      Serial.println(url);
#endif
      sendHTTPSRequest(url);
      delete[] url;
    }
  } else {
#ifdef USESERIAL
    Serial.println("WiFi disconnected");
#endif
  }
}

//@brief -> function that selects outside module and sets its height
void heightSetup() {
  ValueSelector outsideModuleSelector(&display, &setButton, "module");                         //create selector instance to select the index of the outside module we want to change the height of
  int moduleIndex = outsideModuleSelector.selectIntValue(1, OUTSIDE_MODULES_COUNT, 1, 1) - 1;  //index is -1, because it starts from 0
  //Create value selector to select height:
  ValueSelector vs(&display, &setButton, "height", "cm");
  outsideMeasurers[moduleIndex]->setMountingHeight(vs.selectIntValue(outsideMeasurers[moduleIndex]->getMinHeight(), outsideMeasurers[moduleIndex]->getMaxHeight(), outsideMeasurers[moduleIndex]->getMountingHeight(), 5));  //set the new height
  displayController.displayData();                                                                                                                                                                                           //display the standary display output
#ifdef USESERIAL
  Serial.printf("Module: %i, New height: %i\n", moduleIndex + 1, outsideMeasurers[moduleIndex]->getMountingHeight());  //for debugging only
#endif
}

bool selectWifiNetwork() {

  WiFi.mode(WIFI_STA);  //set esp32 as station
  ValueSelector vs(&display, &setButton, "LAN");
  int index = vs.selectStringValueFromArray(savedNetworks, NETWORKS_ARRAY_SIZE, 2);
  int startTime = millis();
  char* ssid = (char*)savedNetworks[index];
  char* pass = (char*)savedNetworks[index + 1];
  WiFi.begin(ssid, pass);
  delay(1000);
  WiFi.disconnect();
  delay(300);
  WiFi.reconnect();
#ifdef USESERIAL
  Serial.print(F("Connecting to WiFi."));
#endif
  while (WiFi.status() != WL_CONNECTED) {
#ifdef USESERIAL
    Serial.print(".");
#endif
      delay(100);
    if (millis() - startTime >= WIFI_CONNECT_TIME_LIMIT) {
#ifdef USESERIAL
      Serial.println(F("Couldn't connect to network"));
#endif
      return false;  //couldn't connect to network
    }
  }
  return true;
}
void setupRadio() {
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setPayloadSize(sizeof(ReceiveBuffer));
  for (int i = 1; i <= OUTSIDE_MODULES_COUNT; i++) {
    radio.openReadingPipe(i, readAddresses[i - 1]);  //open a reading pipe for each of the connected outside modules
  }

  radio.startListening();
}

//@brief gets the current hour from an NTP server
bool getCurrentHour() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
#ifdef USESERIAL
 //   Serial.println("Failed to obtain time");
#endif
    return false;
  }
  currentHour = timeinfo.tm_hour;
  return true;
}


//@brief: Establish connection to the https server and request url
//@params url with data to send
//@returns false if connection is not successful
bool sendHTTPSRequest(const char* url) {
#ifdef USESERIAL
  Serial.println(F("\nStarting connection to server..."));
#endif

  https.begin(url);
  int httpCode = https.GET();
  if (httpCode > 0) {
    String payload = https.getString();
#ifdef USESERIAL
    Serial.println("Response: " + payload);
#endif
  } else {
#ifdef USESERIAL
    Serial.println("HTTP request failed");
    Serial.printf("HTTP code: %i", httpCode);
#endif
    return false;
  }
  https.end();
  return true;
}


//@brief: builds the https request for the receiving web API
const char* buildHTTPSURL() {
  char* httpsURL = new char[500];
  httpsURL[0] = '\0';
  strcat(httpsURL, "https://script.google.com/macros/s/");
  strcat(httpsURL, hostID);
  //convert inside temp/hum to c-string:
  char tempString[10] = "\0";
  dtostrf(insideMeasurer.getTemperature(), 4, 2, tempString);
  char humString[5] = "\0";
  itoa(insideMeasurer.getHumidity(), humString, 10);

  //Append to the url c-string:
  strcat(httpsURL, "/exec?insideTemperature=");
  strcat(httpsURL, tempString);
  strcat(httpsURL, "&insideHumidity=");
  strcat(httpsURL, humString);

  //Append all the outside modules readings to the url c-string:
  for (int i = 0; i < OUTSIDE_MODULES_COUNT; i++) {
    char snowString[5] = "\0";
    char batteryLevelString[5] = "\0";
    dtostrf(outsideMeasurers[i]->getTemperature(), 4, 2, tempString);
    itoa(outsideMeasurers[i]->getHumidity(), humString, 10);
    itoa(outsideMeasurers[i]->getSnowDepth(), snowString, 10);
    itoa(outsideMeasurers[i]->getBatteryLevel(), batteryLevelString, 10);
    char outsideData[50];
    sprintf(outsideData, "&%i,outsideTemperature=%s", i + 1, tempString);  //i+1 is the index of the selected outside module
    strcat(httpsURL, outsideData);
    sprintf(outsideData, "&%i,outsideHumidity=%s", i + 1, humString);
    strcat(httpsURL, outsideData);
    sprintf(outsideData, "&%i,snowDepth=%s", i + 1, snowString);
    strcat(httpsURL, outsideData);
    sprintf(outsideData, "&%i,batLevel=%s", i + 1, batteryLevelString);
    strcat(httpsURL, outsideData);
  }
  return httpsURL;
}