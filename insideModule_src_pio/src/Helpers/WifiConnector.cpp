#include "WiFiConnector.h"

bool WiFiConnector::connectToWifi()
{
    String ssid, pass;
    spiffsManager->readFile(ssidPath, ssid);
    spiffsManager->readFile(passPath, pass);

    if (ssid == "" || pass == "") {
        Serial.println("Undefined SSID or pass.");
        return false;
    }
    WiFi.mode(WIFI_STA); // set esp32 as station

    int startTime = millis();
    WiFi.begin(ssid, pass);
    // Reconnect WiFi once because it breaks the first time when connecting to
    // 5GHz LANs
    delay(1000);
    WiFi.disconnect();
    delay(300);
    WiFi.reconnect();

    Serial.print(F("Connecting to WiFi."));
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
        if (millis() - startTime >= WIFI_CONNECT_TIME_LIMIT) {
            Serial.println(F("Couldn't connect to network"));
            return false; // couldn't connect to network
        }
    }
    Serial.println();
    Serial.print("LAN IP: ");
    Serial.println(WiFi.localIP());
    return true;
}
bool WiFiConnector::startMDNS(const char* mDnsHostName)
{
    // Set up mDNS responder
    if (!MDNS.begin(mDnsHostName)) {
        Serial.println("Error setting up MDNS responder!");
        return false;
    }
    Serial.println("mDNS responder started");
    return true;
}

bool WiFiConnector::startAP()
{
    flag = false;
    // Connect to Wi-Fi network with SSID and password
    Serial.println("Setting AP (Access Point)");
    // NULL sets an open Access Point
    if (WiFi.softAP("ClimaLog-WIFI-MANAGER", NULL)) {
        flag = true;
    }

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    return flag;
}