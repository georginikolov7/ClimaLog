#ifndef WIFICONNECTOR_H
#define WIFICONNECTOR_H
#include "Services/ISpiffsManager.h"
#include "WiFi.h"
#include <Arduino.h>
#include <ESPmDNS.h>
class WiFiConnector {
public:
    WiFiConnector(ISpiffsManager* spiffsManager, const char* ssidPath, const char* passPath)
        : spiffsManager(spiffsManager)
        , ssidPath(ssidPath)
        , passPath(passPath)

    {
    }
    bool connectToWifi();
    bool startMDNS(const char* mDnsHostName);
    bool startAP();
    const char* getDomainName();

private:
    ISpiffsManager* spiffsManager;
    const char* ssidPath;
    const char* passPath;
    bool flag;
    const int WIFI_CONNECT_TIME_LIMIT = 10000; // 10 seconds  for connecting
};
#endif