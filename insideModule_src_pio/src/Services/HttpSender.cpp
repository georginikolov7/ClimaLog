#include "HttpSender.h"

HttpSender::HttpSender(const char* scriptId)
{
    this->scriptId = scriptId;
}
HttpSender::~HttpSender()
{
    delete[] httpURL;
}

// void HttpSender::init(InsideMeasurer* insideMeasurer, OutsideMeasurersRepo* outsideMeasurers)
// {
//     this->insideMeasurer = insideMeasurer;
//     this->outsideMeasurersCount = outsideMeasurers->getCount();
//     this->outsideMeasurers = outsideMeasurers;
// }
void HttpSender::buildHttpURL(InsideMeasurer& insideMeasurer, OutsideMeasurersRepo& outsideMeasurers)
{
    const int URL_LENGTH = 500;
    httpURL = new char[URL_LENGTH]; // string for https url
    httpURL[0] = '\0';
    strncat(httpURL, "https://script.google.com/macros/s/", URL_LENGTH - strlen(httpURL) - 1);
    strncat(httpURL, scriptId, URL_LENGTH - strlen(httpURL) - 1); // append the id part

    // Append inside data to the url c-string:
    strncat(httpURL, "/exec?insideTemperature=", URL_LENGTH - strlen(httpURL) - 1);
    strncat(httpURL, insideMeasurer.getTemperature(), URL_LENGTH - strlen(httpURL) - 1);
    strncat(httpURL, "&insideHumidity=", URL_LENGTH - strlen(httpURL) - 1);
    strncat(httpURL, insideMeasurer.getHumidity(), URL_LENGTH - strlen(httpURL) - 1);
    Serial.println("Check after inside");

    // Append all the outside modules readings to the url c-string:
    for (int i = 0; i < outsideMeasurers.getCount(); i++) {
        // Index in http url is equal to i+1 (because it starts from 1 and not 0)
        OutsideMeasurer* currentMeasurer = &outsideMeasurers[i];
        const int OUTSIDE_DATA_LEN = 50;
        char outsideData[OUTSIDE_DATA_LEN];
        outsideData[0] = '\0';

        // // Append outsideData to http:
        sprintf(outsideData, "&%i,outsideTemperature=%s", i + 1, currentMeasurer->getTemperature());
        strncat(httpURL, outsideData, OUTSIDE_DATA_LEN - strlen(outsideData) - 1);

        snprintf(outsideData, sizeof(outsideData) - 1, "&%i,outsideHumidity=%s", i + 1, currentMeasurer->getTemperature());
        strncat(httpURL, outsideData, OUTSIDE_DATA_LEN - strlen(outsideData) - 1);

        snprintf(outsideData, sizeof(outsideData) - 1, "&%i,snowDepth=%s", i + 1, currentMeasurer->getSnowDepth());
        strncat(httpURL, outsideData, OUTSIDE_DATA_LEN - strlen(outsideData) - 1);

        snprintf(outsideData, sizeof(outsideData) - 1, "&%i,batLevel=%s", i + 1, currentMeasurer->getBatteryLevel());
        strncat(httpURL, outsideData, URL_LENGTH - strlen(httpURL) - 1);
    }

#ifdef USESERIAL
    Serial.printf("HTTP URL: %s\n", httpURL); // print the url for debugging
#endif
}
bool HttpSender::sendRequest(InsideMeasurer& insideMeasurer, OutsideMeasurersRepo& outsideMeasurers)
{
#ifdef USESERIAL
    Serial.println(F("\nStarting connection to server..."));
#endif
    buildHttpURL(insideMeasurer,outsideMeasurers);
    http.begin(httpURL);
    int httpCode = http.GET();
    if (httpCode > 0) {
        String payload = http.getString();
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

    http.end();
    return true;
}