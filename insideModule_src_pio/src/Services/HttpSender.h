#ifndef HTTPSENDER_H
#define HTTPSENDER_H
#include "Models/InsideMeasurer.h"
#include "Models/Measurer.h"
#include "Models/OutsideMeasurer.h"
#include "Repos/OutsideMeasurersRepo.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#define USESERIAL
class HttpSender {
public:
    HttpSender(const char* scriptId);
    ~HttpSender();
    bool sendRequest(InsideMeasurer& insideMeasurer, OutsideMeasurersRepo& outsideMeasurers); // returns true if successfull, otherwise false
private:
    void buildHttpURL(InsideMeasurer& insideMeasurer, OutsideMeasurersRepo& outsideMeasurers);
    HTTPClient http;
    const char* scriptId;
    char* httpURL;
    const int HTTPS_PORT = 443;
    // InsideMeasurer* insideMeasurer;
    // OutsideMeasurersRepo* outsideMeasurers;
};
#endif