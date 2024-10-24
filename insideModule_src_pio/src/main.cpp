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
#include <RF24.h>
#include <SPI.h> //used for communicating with nRF24 radio
#include <nRF24L01.h>
#include <printf.h> //for printing rf24 details (debugging)

// Models:
#include "Models/Button.h"
#include "Models/InsideMeasurer.h"
#include "Models/OutsideMeasurer.h"

// Display:
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "Models/OLEDDisplay.h"
#include "Services/DisplayController.h"
// Services:
#include "Icons.h"
#include "Services/HTTPSender.h"
#include "Services/SpiffsManager.h"
#include "Services/ValueSelector.h"

// Repos:
#include "Repos/OutsideMeasurersRepo.h"

// For sending the data to the web API:
#include "Helpers/WiFiConnector.h"
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <WiFi.h>
#include <time.h>
//-------------------------------------------------------------------------------------------------------------------------------

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

// Outside modules repo:
const char* measurersPath = "/measurers.json";
const int OUTSIDE_MODULES_COUNT = 2; // set the count of the active modules
OutsideMeasurer out1("Out 1", 0, &radio);
OutsideMeasurer out2("Out 2", 1, &radio, true);
OutsideMeasurersRepo outsideMeasurers;
#define DEFAULT_MOUNTING_HEIGHT 80 // modules work best when mounted at 80 cm
// addresses for outside modules (Index+OUTM):
uint8_t readAddresses[6][6] = {
    "1OUTM", "2OUTM", "3OUTM", "4OUTM", "5OUTM", "6OUTM"
};

// SPIFFS:
SpiffsManager spiffsManager;

// WiFi:
const char* mDNSName = "climalogsetup";
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";
AsyncWebServer server(80);
WiFiConnector wiFiConnector(&spiffsManager, ssidPath, passPath);

// Obtaining the time:
const char* ntpServer = "pool.ntp.org";
struct tm currentTime;
struct tm lastTime; // used to store the last time when https request was sent
const long gmtOffset_sec = 7200; // UTC +2 hours id +7200 seconds
const int daylightOffset_sec = 0; // not summer time

// Google apps script:
const char* hostID = "AKfycby3nkAQKVJ2M6Oo7USsUUVcAEmSNu6NRPp86zZgQFSBUEhfyxwjZn7Erk3E3MQoIdAYiQ"; // the id of the google apps script

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
//@brief - >reads info from spiffs and creates measurers
void createOutsideMeasurers();
//@brief -> function that selects outside module and sets its height
void heightSetup();

//@brief -> function that sets device to wifi acces point and reads setup info from mobile device
void configWebDevice();

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

    // Initialize I2C comm and display object:
    Wire.begin(I2C_DATA, I2C_CLK, 100000);
    display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS);
    display.defaultSetup();
    display.writeText("Booting...");

    // Start up spiffs:
    spiffsManager.initSpiffs();

    // Add measurers:
    // Outside measurer instances:

    outsideMeasurers.initRepo(6);
    outsideMeasurers.add(&out1);
    outsideMeasurers.add(&out2);

    // EEPROM used to save mounting height
    EEPROM.begin(
        OUTSIDE_MODULES_COUNT); // we need to keep one int value (mounting height) for every outside module

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
    // config WiFI and web server:
    configWebDevice();
    delay(500);

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    // Obtain current time:
    if (!getLocalTime(&currentTime)) {
        Serial.println("Failed to obtain time from NTC");
    } else {
        Serial.printf("\n%i:%i\n", currentTime.tm_hour, currentTime.tm_min);
    }

    SPI.begin();

    insideDHT.begin(); // initialize the inside DHT
    delay(500); // time to start up DHT
    insideMeasurer.readValues();
    // Init the display controller:
    displayController.begin(&insideMeasurer, &outsideMeasurers);
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
    Serial.printf("Outside modules: %i\r\n", outsideMeasurers.getCount());
    // Serial.println(F("RF24 settings:"));
    // radio.printPrettyDetails();
    Serial.println(F("Setup is complete!"));
    // delay(3000);
    // Serial.println((*outsideMeasurers)[1].getName());
}

void loop()
{
    //     // Check if data is received:
    if (radio.available(&receivePipe)) {
        // receivePipe corresponds to index of outsideMeasurer + 1
        outsideMeasurers[receivePipe - 1].readValues();
#ifdef USESERIAL
        Serial.printf("Received data from outside module %i\n", receivePipe); // for debugging
#endif
        displayController.displayData(); // update values on display
    }

    if (setButton.isShortPressed()) {
        display.clearDisplay();
        char output[100] = "";
        strncat(output, "Host:\n", strlen(output) - 1);
        strcat(output, mDNSName);
        strcat(output, ".local");

        display.writeText(output);
    }
    if (setButton.isLongPressed()) {
        // Enter height setup:
        heightSetup();
    }

    // Change WiFi network by longpressing display btn:
    if (displayButton.isLongPressed()) {
        // Clear wifi credentials and restart device to enter new credentials:
        spiffsManager.writeToFile(ssidPath, "");
        spiffsManager.writeToFile(passPath, "");
        ESP.restart();
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

                Serial.printf("Sending http request. Current time: %i:%i\n\n",
                    currentTime.tm_hour, currentTime.tm_min);
                if (httpSender->sendRequest(insideMeasurer, outsideMeasurers)) {
                    HTTPS_sent = true;
                }
                // Clear memory from httpSender
                delete httpSender;
            }
        } else {
            Serial.println("Error! Could not get time from ntp server!");
        }
    } else {
        display.drawBitmap(display.getWidth() - NO_WIFI_WIDTH, BATTERY_INDICATOR_HEIGHT + 5, epd_bitmap_no_wifi, NO_WIFI_WIDTH, NO_WIFI_HEIGHT);
    }
}

// void createOutsideMeasurers()
// {
//     JsonDocument doc;
//     String json;
//     spiffsManager->readFile(measurersPath, json);

//     Serial.println(json);
//     DeserializationError error = deserializeJson(doc, json);
//     if (error) {
//         Serial.println(error.c_str());
//     }
//     // Access the data
//     for (JsonVariant element : doc.as<JsonArray>()) {
//         const char* name = element["name"];
//         int height = element["height"];
//         const char* power = element["power"];
//         Serial.println(name);
//         Serial.println(height);
//         Serial.println(power);

//         bool isPluggedIn;
//         if (power == "battery") {
//             isPluggedIn = true;
//         } else {
//             isPluggedIn = false;
//         }
//         OutsideMeasurer* measurer = new OutsideMeasurer(name, height, &radio, isPluggedIn);
//         outsideMeasurers.add(*measurer);
//         Serial.printf("Count: %i\r\n", outsideMeasurers.getCount());
//     }
// }
//@brief: setup web server callbacks and wifi settings
void configWebDevice()
{
    display.resetDisplay();
    display.writeText("Connecting to WiFi");
    // Try to connect WiFi:
    if (wiFiConnector.connectToWifi()) {
        // Start up MDNS:
        // Route for root / web page
        server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
            request->send(SPIFFS, "/setupPage.html", "text/html", false);
        });
        server.on("/api/data", HTTP_GET, [](AsyncWebServerRequest* request) {
            // Convert measurers to JSON:
            String json;
            JsonDocument jsonDocument;
            JsonArray jsonArray = jsonDocument.to<JsonArray>();
            for (int i = 0; i < outsideMeasurers.getCount(); i++) {
                OutsideMeasurer& currentMeasurer = outsideMeasurers[i];
                JsonObject obj = jsonArray.add<JsonObject>();
                obj["name"] = currentMeasurer.getName();
                obj["height"] = currentMeasurer.getMountingHeight();
                obj["power"] = currentMeasurer.getPowerSource();
            }
            serializeJson(jsonDocument, json);
            Serial.println(json);
            // Send request:
            request->send(200, "application/json", json);
        });

        server.serveStatic("/", SPIFFS, "/");

        server.on(
            "/api/data", HTTP_POST, [](AsyncWebServerRequest* request) {}, NULL, [](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
                Serial.println((const char*)data);
                 //Save json:
                 spiffsManager.writeToFile(measurersPath,(const char*)data);
                 request->send(200);
                 display.resetDisplay();
                 display.writeText("Not functional yet!");
                 delay(2000);
            server.end();
            ESP.restart(); });
    } else {
        // Put web server into wifi select mode:
        display.resetDisplay();
        display.writeText("Open wifi setup\nConsult manual");
        if (!wiFiConnector.startAP()) {
            Serial.println("Error occurred! Could not init WiFi AP");
            display.resetDisplay();
            display.writeText("Error!");
        }

        // Get request to main dir returns wifiSetup html page
        server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
            request->send(SPIFFS, "/WiFiManager.html", "text/html");
        });

        server.serveStatic("/", SPIFFS, "/");

        server.on("/", HTTP_POST, [](AsyncWebServerRequest* request) {
            int params = request->params();
            for (int i = 0; i < params; i++) {
                AsyncWebParameter* p = request->getParam(i);
                String value = p->value();
                if (p->name() == "ssid") {
                    // Write ssid to flash:
                    spiffsManager.writeToFile(ssidPath, value.c_str());
                } else if (p->name() == "pass") {
                    // Write pass to flash:
                    spiffsManager.writeToFile(passPath, value.c_str());
                }
            }
            request->send(200, "text/plain", "SSID and password read successfully. Server is closed. Device will autorestart and connect to wifi!");
            server.end();
            delay(2000);
            ESP.restart();
        });
    }
    // Start mDNS:
    if (!wiFiConnector.startMDNS(mDNSName)) {
        // TODO
    }
    server.begin();
}
void setupRadio()
{
    radio.setDataRate(RF24_250KBPS);
    radio.setPALevel(RF24_PA_MAX, true);
    radio.setPayloadSize(sizeof(ReceiveBuffer));

    // open a reading pipe for each of the connected outside modules:
    for (int i = 1; i <= 6; i++) {
        radio.openReadingPipe(i, readAddresses[i - 1]);
    }
    // Set radio in read mode:
    radio.startListening();
}

void heightSetup()
{
    Serial.println(outsideMeasurers[0].getName());
    Serial.println(outsideMeasurers[1].getName());
    // get array holding the names of the modules :
    char names[OUTSIDE_MODULES_COUNT][200];
    for (int i = 0; i < OUTSIDE_MODULES_COUNT; i++) {
        strcpy(names[i], outsideMeasurers[i].getName());
    }

    // create valueSelector for getting the index of the module:
    ValueSelector outsideModuleSelector(&display, &setButton, "module");
    int moduleIndex = outsideModuleSelector.selectStringValueFromArray(names, OUTSIDE_MODULES_COUNT, 1);
    // Create value selector to select height:
    ValueSelector vs(&display, &setButton, "height", "cm");

    OutsideMeasurer selectedMeasurer = outsideMeasurers[moduleIndex];
    selectedMeasurer.setMountingHeight(
        vs.selectIntValue(selectedMeasurer.getMinHeight(), selectedMeasurer.getMaxHeight(), selectedMeasurer.getMountingHeight(), 5)); // set the new height

    displayController.displayData(); // display the standary display output
    Serial.printf(
        "Module: %i, New height: %i\n", moduleIndex + 1,
        selectedMeasurer.getMountingHeight()); // for debugging only
}

// Brooks was here...

// So was Red.