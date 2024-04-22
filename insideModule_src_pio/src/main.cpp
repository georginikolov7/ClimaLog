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
#include <DHT.h>
#include <EEPROM.h> //used to store settings when power goes down (mounting height of outside module)
#include <RF24.h>
#include <SPI.h> //used for communicating with nRF24 radio
#include <nRF24L01.h>
#include <printf.h> //for printing rf24 details (debugging)

// Models:
#include "Models/Button.h"
#include "Models/InsideMeasurer.h"
#include "Models/OutsideMeasurer.h"
#include "Models/WifiNetwork.h"

// Display:
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "Models/OLEDDisplay.h"
#include "Services/DisplayController.h"
// Services:
#include "Icons.h"
#include "Services/HTTPSender.h"
#include "Services/ValueSelector.h"

// Repos:
#include "Repos/NetworkRepo.h"
#include "Repos/OutsideMeasurersRepo.h"
// For sending the data to the web API:
#include <WiFi.h>
#include <time.h>
//-------------------------------------------------------------------------------------------------------------------------------

#define USESERIAL // uncomment to use serial monitor for debugging

// DHT22 Sensor:
#define INSIDE_DHT_TYPE DHT22
#define INSIDE_DHT_PIN 13 // DHT22 pin
DHT insideDHT(INSIDE_DHT_PIN, INSIDE_DHT_TYPE);

// Inside measurer instance:
InsideMeasurer insideMeasurer("In", &insideDHT);

// OLED display:
#define I2C_DATA 21 // I2C pin for data
#define I2C_CLK 22 // I2C pin for clock
#define WIDTH 128 // width of display in px
#define HEIGHT 64 // height of display in px
#define DISPLAY_ADDRESS 0x3c // I2C address for OLED display
OLEDDisplay display(WIDTH, HEIGHT, &Wire); // OLED display object

// Button objects:
#define SET_BUTTON_PIN 33
#define DISPLAY_BUTTON_PIN 32
Button setButton(SET_BUTTON_PIN);
Button displayButton(DISPLAY_BUTTON_PIN);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// NOTE: AS OF THIS MOMENT, MODULES AND WIFI NETWORKS NEED TO BE ADDED MANUALLY HERE IN THE CODE. DYNAMIC SETUP VIA BLUETOOTH IS UNDERWAY!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Radio:
#define CE_PIN 17 // SPI chip enable
#define CS_PIN 5 // SPI chip select pin
uint8_t receivePipe; // used to store the pipe num where data was received
RF24 radio(CE_PIN, CS_PIN); // radio object

// Outside modules:
const int OUTSIDE_MODULES_COUNT = 2; // set the count of the active modules
// addresses for outside modules (Index+OUTM):
uint8_t readAddresses[6][6] = {
    "1OUTM", "2OUTM", "3OUTM", "4OUTM", "5OUTM", "6OUTM"
};
#define DEFAULT_MOUNTING_HEIGHT 80 // modules work best when mounted at 80 cm
// Outside measurer instances:
OutsideMeasurer out1("Out 1", 0, &radio);
OutsideMeasurer out2("Out 2", 1, &radio, true);
// OutsideMeasurer repo:
OutsideMeasurersRepo outsideMeasurers(6);

// WiFi:
const int WIFI_CONNECT_TIME_LIMIT = 40000; // 40 seconds  for connecting
#define MAX_NETWORKS_COUNT 10
NetworkRepo networks(MAX_NETWORKS_COUNT);
WifiNetwork phone("AndroidAPe456", "12345678");
WifiNetwork dumy1("pesho's wifi", "*******");
WifiNetwork dumy2("Nokolovi UBMT", "bvnSP0620");

// Obtaining the time:
const char* ntpServer = "pool.ntp.org";
struct tm currentTime;
struct tm lastTime; // used to store the last time when https request was sent
const long gmtOffset_sec = 7200; // UTC +2 hours id +7200 seconds
const int daylightOffset_sec = 0; // not summer time

// HTTP:
const char* hostID = "AKfycby3nkAQKVJ2M6Oo7USsUUVcAEmSNu6NRPp86zZgQFSBUEhfyxwjZn7Erk3E3MQoIdAYiQ"; // the id of the google apps script
// HttpSender httpSender(hostID);

// Display controller object, used for toggling display mode:
DisplayController displayController(&display);

// Control variables and constants:
bool HTTPS_sent = true;
const int HTTP_SEND_INTERVAL = 1; // send request every minute
unsigned long lastInsideReadTime = 0;
#define INSIDE_READ_INTERVAL 120000 // read temp/hum inside once every 2 mins

// Interrupts for the two buttons:
void IRAM_ATTR ISR_SET_BUTTON() { setButton.stateChanged(); }
void IRAM_ATTR ISR_DISPLAY_BUTTON() { displayButton.stateChanged(); }

//-------------------------------------------------------------------------------------------------------------
// Functions:
//@brief -> function that selects outside module and sets its height
void heightSetup();

//@brief -> function that selects one of the preset wifi networks
bool selectWifiNetwork();

//@Brief: Sets initial settings to nrf24 radio module
void setupRadio();
//-------------------------------------------------------------------------------------------------------------

void setup()
{
    // put your setup code here, to run once:

    // start up the serial communication:
#ifdef USESERIAL
    Serial.begin(115200);
    while (!Serial) {
    }
#endif

    // Attach interrupts for buttons:
    attachInterrupt(SET_BUTTON_PIN, ISR_SET_BUTTON, CHANGE);
    attachInterrupt(DISPLAY_BUTTON_PIN, ISR_DISPLAY_BUTTON, CHANGE);

    // add Measurers to repo :
    outsideMeasurers.add(out1);
    outsideMeasurers.add(out2);
    // Add networks to repo:
    networks.add(phone);
    networks.add(dumy1);
    networks.add(dumy2);

    // Initialize I2C comm and display object:
    Wire.begin(I2C_DATA, I2C_CLK, 100000);
    display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS);
    display.defaultSetup();
    display.writeText("Booting...");

    // Init the display controller:
    displayController.begin(&insideMeasurer, &outsideMeasurers, OUTSIDE_MODULES_COUNT);
    delay(700);

    // Establish Wifi connection/config time settings:
    selectWifiNetwork();
    delay(500);
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    // Obtain current time:
    if (!getLocalTime(&currentTime)) {
#ifdef USESERIAL
        Serial.println("Failed to obtain time from NTC");
#endif
    } else {
#ifdef USESERIAL
        Serial.printf("\n%i:%i\n", currentTime.tm_hour, currentTime.tm_min);
#endif
    }

    SPI.begin();

    // EEPROM used to save mounting height
    EEPROM.begin(OUTSIDE_MODULES_COUNT); // we need to keep one int value (mounting height) for every outside module

    // Set mounting heights on each outside module:
#ifdef USESERIAL
    Serial.println("Reading stored mounting heights");
#endif
    for (int i = 0; i < OUTSIDE_MODULES_COUNT; i++) {
        int heightFromFlash = EEPROM.read(i);
        if (heightFromFlash == 255) {
            // height hasn't been set yet:
            outsideMeasurers[i].setMountingHeight(DEFAULT_MOUNTING_HEIGHT);
        } else {
            // set height from flash:
            outsideMeasurers[i].setMountingHeight(heightFromFlash);
        }
    }
#ifdef USESERIAL
    Serial.println("Stored mounting heights read");
#endif
    insideDHT.begin(); // initialize the inside DHT
    delay(500); // time to start up DHT
    insideMeasurer.readValues();

    displayController.displayData(); // displays first measurer in array (inside measurer by default)

    // initialize RF24 radio:
    if (!radio.begin()) {
#ifdef USESERIAL
        Serial.println(F("Could not initialize RF24 module!"));
#endif
        display.println("ERROR!\nCANNOT INIT RF24!");
        while (true) {
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
    // Check if data is received:
    if (radio.available(&receivePipe)) {
        // receivePipe corresponds to index of outsideMeasurer + 1
        outsideMeasurers[receivePipe - 1].readValues();
#ifdef USESERIAL
        Serial.printf("Received data from outside module %i\n", receivePipe); // for debugging
#endif
        displayController.displayData(); // update values on display
    }

    if (setButton.isLongPressed()) {
        // Enter height setup:
        heightSetup();
    }

    // Change WiFi network by longpressing display btn:
    if (displayButton.isLongPressed()) {
        selectWifiNetwork();
        displayController.displayData();
    }

    // change displayed data if button is pressed:
    if (displayButton.isShortPressed()) {
        displayController.changeDisplayMode();
        displayController.displayData();
#ifdef USESERIAL
        Serial.printf("CHANGED MODE: %i\n", displayController.getCurrentIndex());
#endif
    }

    // Read inside values if time has passed:
    if (millis() - lastInsideReadTime > INSIDE_READ_INTERVAL) {
        insideMeasurer.readValues();
        displayController.displayData(); // if inside module is showing on display
                                         // => update values
        lastInsideReadTime = millis();
    }

    // Send http request if time has elapsed or it is midnight:
    if (WiFi.status() == WL_CONNECTED) {
        if (getLocalTime(&currentTime)) {
            int elapsedMins = (currentTime.tm_hour * 60 + currentTime.tm_min) - (lastTime.tm_hour * 60 + lastTime.tm_min);
            if (elapsedMins >= HTTP_SEND_INTERVAL || (currentTime.tm_hour == 0 && lastTime.tm_hour == 23)) {
                HTTPS_sent = false;
                lastTime.tm_hour = currentTime.tm_hour;
                lastTime.tm_min = currentTime.tm_min;
            }

            // Send https request:
            if (!HTTPS_sent) {
                HttpSender* httpSender = new HttpSender(hostID);
#ifdef USESERIAL
                Serial.printf("Sending http request. Current time: %i:%i\n\n",
                    currentTime.tm_hour, currentTime.tm_min);
#endif
                if (httpSender->sendRequest(insideMeasurer, outsideMeasurers)) {
                    HTTPS_sent = true;
                }
                // Clear memory from httpSender
                delete httpSender;
            }
        } else {
#ifdef USESERIAL
            Serial.println("Error! Could not get time from ntp server!");
#endif
        }
    } else {
#ifdef USESERIAL
        Serial.println("Error! WiFi disconnected!");
#endif
    }
}

void setupRadio()
{
    radio.setDataRate(RF24_250KBPS);
    radio.setPALevel(RF24_PA_MAX, true);
    radio.setPayloadSize(sizeof(ReceiveBuffer));

    // open a reading pipe for each of the connected outside modules:
    for (int i = 1; i <= OUTSIDE_MODULES_COUNT; i++) {
        radio.openReadingPipe(i, readAddresses[i - 1]);
    }

    // Set radio in read mode:
    radio.startListening();
}
void heightSetup()
{
    // get array holding the names of the modules:
    char names[OUTSIDE_MODULES_COUNT][200];
    for (int i = 0; i < OUTSIDE_MODULES_COUNT; i++) {
        strcpy(names[i], outsideMeasurers[i].getName());
    }

    // create valueSelector for getting the index of the module:
    ValueSelector outsideModuleSelector(&display, &setButton, "module");
    int moduleIndex = outsideModuleSelector.selectStringValueFromArray(names, OUTSIDE_MODULES_COUNT, 1);
    // Create value selector to select height:
    ValueSelector vs(&display, &setButton, "height", "cm");
    outsideMeasurers[moduleIndex].setMountingHeight(
        vs.selectIntValue(outsideMeasurers[moduleIndex].getMinHeight(), outsideMeasurers[moduleIndex].getMaxHeight(), outsideMeasurers[moduleIndex].getMountingHeight(), 5)); // set the new height
    displayController.displayData(); // display the standary display output
#ifdef USESERIAL
    Serial.printf(
        "Module: %i, New height: %i\n", moduleIndex + 1,
        outsideMeasurers[moduleIndex].getMountingHeight()); // for debugging only
#endif
}

bool selectWifiNetwork()
{
    WiFi.mode(WIFI_STA); // set esp32 as station
    ValueSelector vs(&display, &setButton, "LAN"); // select LAN from the saved networks array

    int size = networks.getCount();
    char namesArray[size][200];

    for (int i = 0; i < size; i++) {
        strcpy(namesArray[i], networks[i].getSSID());
    }
    int index = vs.selectStringValueFromArray(namesArray, size, 1); // get the index of the selected ssid
    int startTime = millis();
    char* ssid = (char*)networks[index].getSSID();
    char* pass = (char*)networks[index].getPassword();
    WiFi.begin(ssid, pass);
    // Reconnect WiFi once because it breaks the first time when connecting to
    // 5GHz LANs
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
            return false; // couldn't connect to network
        }
    }
    return true;
}
// Brooks was here...

// So was Red.