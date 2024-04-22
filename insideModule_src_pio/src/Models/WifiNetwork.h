#ifndef WIFINETWORK_H
#define WIFINETWORK_H
#include "Arduino.h"
class WifiNetwork {
public:
    WifiNetwork() = default;
    WifiNetwork(const char* SSID, const char* password);
    ~WifiNetwork();
    const char* getSSID() const;
    const char* getPassword() const;
    bool operator==(WifiNetwork& other);

private:
    char* ssid;
    char* password;
};
#endif