//************************************************************************************************
//@Brief:
//- Receives data from outside module
//- Reads temp/hum inside
//- Display data on OLED display
//- Send data to Google Apps script (HTTPS)

//@Cotroller: ESP32 WROOM D2

//************************************************************************************************

// Adding necessary libraries:
#include <Arduino.h>
#include <EEPROM.h> //used to store settings when power goes down (mounting height of outside module)
#include <printf.h> //for printing rf24 details (debugging)
#include <SPI.h>    //used for communicating with nRF24 radio
#include <DHT.h>
#include <RF24.h>
#include <nRF24L01.h>
#include "InsideMeasurer.h"
#include "OutsideMeasurer.h"
#include "Button.h"
#include "OLEDDisplay.h"
#include "DisplayController.h"
#include "ValueSelector.h"

// For sending the data to the web API:
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>

//-------------------------------------------------------------------------------------------------------------------------------

#define USESERIAL // uncomment to use serial monitor for debugging

// WiFi:
const int WIFI_CONNECT_TIME_LIMIT = 60000; // 1 minute for connecting
#define SAVED_NETWORKS_COUNT 4
const int NETWORKS_ARRAY_SIZE = SAVED_NETWORKS_COUNT * 2;
// array for holding wifi networks (even index->ssid, odd index-> pass )
const char savedNetworks[][50] = {
    "kartala", "kartala1333", "AndroidAPe456", "12345678", "VIVACOM 4G WiFi_2C8D", "00505022", "Nokolovi UBMT", "bvnSP0620"};

const int httpsPort = 443;
const char *hostID = "AKfycby3nkAQKVJ2M6Oo7USsUUVcAEmSNu6NRPp86zZgQFSBUEhfyxwjZn7Erk3E3MQoIdAYiQ"; // the id of the google apps script
HTTPClient https;                                                                                // client object

// Obtaining the time:
struct tm currentTime;
struct tm lastTime; // used to store the last time when https request was sent

const long gmtOffset_sec = 7200;  // UTC +2 hours id +7200 seconds
const int daylightOffset_sec = 0; // not summer time

const char *ntpServer = "pool.ntp.org";

// DHT22 Sensor:
#define INSIDE_DHT_TYPE DHT22
#define INSIDE_DHT_PIN 13                       // DHT22 pin
DHT insideDHT(INSIDE_DHT_PIN, INSIDE_DHT_TYPE); // dht22 temp/hum sensor for inside
InsideMeasurer insideMeasurer(&insideDHT);      // create instance for measuring temp/humidity inside

// OLED display:
#define I2C_DATA 21                        // I2C pin for data
#define I2C_CLK 22                         // I2C pin for clock
#define WIDTH 128                          // width of display in px
#define HEIGHT 64                          // height of display in px
#define DISPLAY_ADDRESS 0x3c               // I2C address for OLED display
OLEDDisplay display(WIDTH, HEIGHT, &Wire); // OLED display object

// Button objects:
#define SET_BUTTON_PIN 33
#define DISPLAY_BUTTON_PIN 32
Button setButton(SET_BUTTON_PIN);
Button displayButton(DISPLAY_BUTTON_PIN);

// Outside modules:

//! WARNING Modules must send at differrent times eg -> 1st module every 5mins, 2nd module every 7 mins
#define OUTSIDE_MODULES_COUNT 1 // set the count of the active modules
// Radio:
#define CE_PIN 17                                            // SPI chip enable
#define CS_PIN 5                                             // SPI chip select pin
uint8_t readAddresses[OUTSIDE_MODULES_COUNT][6] = {"1OUTM"}; // addresses for outside modules (Index+OUTM)
uint8_t receivePipe;                                         // used to store the pipe num where data was received
RF24 radio(CE_PIN, CS_PIN);                                  // radio object

#define DEFAULT_MOUNTING_HEIGHT 80          // modules work best when mounted at 80 cm
OutsideMeasurer outsideMeasurer(&radio, 1); // create instance for receiving data from radio module (outside data)
// OutsideMeasurer out2(&radio, 2);

// OutsideMeasurer pointer to array:
OutsideMeasurer *outsideMeasurers[OUTSIDE_MODULES_COUNT] = {
    &outsideMeasurer};

// Display controller object, used for toggling display mode
DisplayController displayController(&display, &insideMeasurer, outsideMeasurers, OUTSIDE_MODULES_COUNT);

// Control variables and constants:
bool HTTPS_sent = true;
const int HTTP_SEND_INTERVAL = 15; // send request every 30 minutes
unsigned long lastInsideReadTime = 0;
#define INSIDE_READ_INTERVAL 120000 // read temp/hum inside once every 2 mins

// Interrupts for the two buttons:
void IRAM_ATTR ISR_SET_BUTTON()
{
  setButton.stateChanged();
}
void IRAM_ATTR ISR_DISPLAY_BUTTON()
{
  displayButton.stateChanged();
}

// Functions:
//@brief -> function that selects outside module and sets its height
void heightSetup()
{
  ValueSelector outsideModuleSelector(&display, &setButton, "module");                        // create selector instance to select the index of the outside module we want to change the height of
  int moduleIndex = outsideModuleSelector.selectIntValue(1, OUTSIDE_MODULES_COUNT, 1, 1) - 1; // index is -1, because it starts from 0
  // Create value selector to select height:
  ValueSelector vs(&display, &setButton, "height", "cm");
  outsideMeasurers[moduleIndex]->setMountingHeight(vs.selectIntValue(outsideMeasurers[moduleIndex]->getMinHeight(), outsideMeasurers[moduleIndex]->getMaxHeight(), outsideMeasurers[moduleIndex]->getMountingHeight(), 5)); // set the new height
  displayController.displayData();                                                                                                                                                                                          // display the standary display output
#ifdef USESERIAL
  Serial.printf("Module: %i, New height: %i\n", moduleIndex + 1, outsideMeasurers[moduleIndex]->getMountingHeight()); // for debugging only
#endif
}

bool selectWifiNetwork()
{

  WiFi.mode(WIFI_STA);                                                              // set esp32 as station
  ValueSelector vs(&display, &setButton, "LAN");                                    // select LAN from the saved networks array
  int index = vs.selectStringValueFromArray(savedNetworks, NETWORKS_ARRAY_SIZE, 2); // get the index of the selected ssid
  int startTime = millis();
  char *ssid = (char *)savedNetworks[index];
  char *pass = (char *)savedNetworks[index + 1];
  WiFi.begin(ssid, pass);
  // Reconnect WiFi once because it breaks the first time when connecting to 5GHz LANs
  delay(1000);
  WiFi.disconnect();
  delay(300);
  WiFi.reconnect();

#ifdef USESERIAL
  Serial.print(F("Connecting to WiFi."));
#endif
  while (WiFi.status() != WL_CONNECTED)
  {
#ifdef USESERIAL
    Serial.print(".");
#endif
    delay(100);
    if (millis() - startTime >= WIFI_CONNECT_TIME_LIMIT)
    {
#ifdef USESERIAL
      Serial.println(F("Couldn't connect to network"));
#endif
      return false; // couldn't connect to network
    }
  }
  return true;
}

//@Brief: Sets initial settings to nrf24 radio module
void setupRadio()
{
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX,true);
  radio.setPayloadSize(sizeof(ReceiveBuffer));
  // radio.setRetries

  // open a reading pipe for each of the connected outside modules:
  for (int i = 1; i <= OUTSIDE_MODULES_COUNT; i++)
  {
    radio.openReadingPipe(i, readAddresses[i - 1]);
  }

  // Set radio in read mode:
  radio.startListening();
}


//@brief: Establish connection to the https server and request url
//@params url with data to send
//@returns false if connection is not successful
bool sendHTTPSRequest(const char *url)
{
#ifdef USESERIAL
  Serial.println(F("\nStarting connection to server..."));
#endif

  https.begin(url);
  int httpCode = https.GET();
  if (httpCode > 0)
  {
    String payload = https.getString();
#ifdef USESERIAL
    Serial.println("Response: " + payload);
#endif
  }
  else
  {
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
const char *buildHTTPSURL()
{
  char *httpsURL = new char[500]; // string for https url
  httpsURL[0] = '\0';
  strcat(httpsURL, "https://script.google.com/macros/s/");
  strcat(httpsURL, hostID); // append the id part

  // convert inside temp/hum to c-string:
  char tempString[10] = "\0";
  dtostrf(insideMeasurer.getTemperature(), 4, 2, tempString);
  char humString[5] = "\0";
  itoa(insideMeasurer.getHumidity(), humString, 10);

  // Append inside data to the url c-string:
  strcat(httpsURL, "/exec?insideTemperature=");
  strcat(httpsURL, tempString);
  strcat(httpsURL, "&insideHumidity=");
  strcat(httpsURL, humString);

  // Append all the outside modules readings to the url c-string:
  for (int i = 0; i < OUTSIDE_MODULES_COUNT; i++)
  {
    // convert outside data to c-strings:
    char snowString[5] = "\0";
    char batteryLevelString[5] = "\0";
    dtostrf(outsideMeasurers[i]->getTemperature(), 4, 2, tempString);
    itoa(outsideMeasurers[i]->getHumidity(), humString, 10);
    itoa(outsideMeasurers[i]->getSnowDepth(), snowString, 10);
    itoa(outsideMeasurers[i]->getBatteryLevel(), batteryLevelString, 10);

    // Append the data to the url:
    // Index in http url is equal to i+1 (because it starts from 1 and not 0)
    char outsideData[50];
    sprintf(outsideData, "&%i,outsideTemperature=%s", i + 1, tempString);
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

void setup()
{
  // put your setup code here, to run once:

  // start up the serial communication:
#ifdef USESERIAL
  Serial.begin(115200);
  while (!Serial)
  {
  }
#endif

  // Attach interrupts for buttons:
  attachInterrupt(SET_BUTTON_PIN, ISR_SET_BUTTON, CHANGE);
  attachInterrupt(DISPLAY_BUTTON_PIN, ISR_DISPLAY_BUTTON, CHANGE);

  // Initialize I2C comm and display object:
  Wire.begin(I2C_DATA, I2C_CLK, 100000);
  display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS);
  display.setupDisplay();
  display.resetDisplay();
  display.writeText("Booting...");
  delay(700);

  // Establish Wifi connection/config time settings:
  selectWifiNetwork();
  delay(500);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Obtain current time:
  if (!getLocalTime(&currentTime))
  {
#ifdef USESERIAL
    Serial.println("Failed to obtain time from NTC");
#endif
  }

  SPI.begin();

  // EEPROM used to save mounting height
  EEPROM.begin(OUTSIDE_MODULES_COUNT); // we need to keep one int value (mounting height) for every outside module

  // Set mounting heights on each outside module:
  for (int i = 0; i < OUTSIDE_MODULES_COUNT; i++)
  {
    int heightFromFlash = EEPROM.read(i);
    if (heightFromFlash == 255)
    {
      // height hasn't been set yet:
      outsideMeasurers[i]->setMountingHeight(DEFAULT_MOUNTING_HEIGHT);
    }
    else
    {
      // set height from flash:
      outsideMeasurers[i]->setMountingHeight(heightFromFlash);
    }
  }

  insideDHT.begin(); // initialize the inside DHT
  delay(500);        // time to start up DHT
  insideMeasurer.readValues();

  displayController.displayData(); // displays first measurer in array (inside measurer by default)

  // initialize RF24 radio:
  if (!radio.begin())
  {
#ifdef USESERIAL
    Serial.println(F("Could not initialize RF24 module!"));
#endif
    display.writeText("ERROR!\nCANNOT INIT RF24!");
    while (true)
    {
      // Do nothing...
    }
  }
  setupRadio();

// Debugging info:
#ifdef USESERIAL
  Serial.println(F("RF24 settings:"));
  radio.printPrettyDetails();
  Serial.print(F("Wifi IP Adress: "));
  Serial.println(WiFi.localIP());
  Serial.println(F("Setup is complete!"));
#endif
}

void loop()
{
  // Check if command is available on Serial (for testing)
  String readCommand;
  if (Serial.available() > 0)
  {
    readCommand = Serial.readStringUntil('\n');
  }

  // Check if data is received:
  if (radio.available(&receivePipe))
  {
    // receivePipe corresponds to index of outsideMeasurer + 1
    outsideMeasurers[receivePipe - 1]->readValues();
#ifdef USESERIAL
    Serial.printf("Received data from outside module %i\n", receivePipe); // for debuging
    Serial.println(outsideMeasurer.getOutput());
    Serial.printf("Battery at: %i\n", outsideMeasurer.getBatteryLevel());
#endif
    displayController.displayData(); // update values on display
  }

  if (setButton.isLongPressed())
  {
    // Enter height setup:
    heightSetup();
  }

  // change displayed data if button is pressed:
  if (displayButton.isShortPressed())
  {
    displayController.changeDisplayMode();
    displayController.displayData();
#ifdef USESERIAL
    Serial.printf("CHANGED MODE: %i\n", displayController.getIterator());
#endif
  }

  // Read inside values if time has passed:
  if (millis() - lastInsideReadTime > INSIDE_READ_INTERVAL)
  {
    insideMeasurer.readValues();
    displayController.displayData(); // if inside module is showing on display => update values
    lastInsideReadTime = millis();
  }

  if (WiFi.status() == WL_CONNECTED)
  {

    if (getLocalTime(&currentTime)){
        
      int elapsedMins = (currentTime.tm_hour * 60 + currentTime.tm_min) - (lastTime.tm_hour * 60 + lastTime.tm_min);

      //Send http request if time has elapsed or it is midnight
      if (elapsedMins >= HTTP_SEND_INTERVAL || (currentTime.tm_hour==0 && lastTime.tm_hour==23))
      {
        HTTPS_sent = false;
        lastTime.tm_hour = currentTime.tm_hour;
        lastTime.tm_min = currentTime.tm_min;
      }
      // Send https request:
      if (!HTTPS_sent || readCommand == "HTTPS")
      {

        #ifdef USESERIAL
  Serial.printf("Sending http request. Current time: %i:%i\n\n", currentTime.tm_hour, currentTime.tm_min);
#endif
        // build the https url:
        const char *url = buildHTTPSURL();
#ifdef USESERIAL
        Serial.println(url);
#endif
        if (sendHTTPSRequest(url))
        {
          HTTPS_sent = true;
        }
        delete[] url;
      }
    }
    else
    {
#ifdef USESERIAL
      Serial.println("Error! Could not get time from ntp server!");
#endif
    }
  }
  else
  {
#ifdef USESERIAL
    Serial.println("Error! WiFi disconnected!");
#endif
  }
}



























// Brooks was here...

// And so was Red.