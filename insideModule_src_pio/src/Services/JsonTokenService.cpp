#include "Services/JsonTokenService.h"

WifiNetwork* JsonTokenService::parseNetwork(const char* json1)
{

    // JSON: {}
    //const char* json = "{\"ssid\":\"pesho\",\"password\":\"123\"}";
    Serial.printf("Raw json: %s\n", json1);
    Serial.println("Parsing network...");

    JsonDocument jsonDoc;
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(jsonDoc, json1);

    // Test if parsing succeeds.
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
    }
    //deserializeJson(jsonDoc, json1);
    Serial.println("deserialized json");

    // WifiNetwork* network = new WifiNetwork(jsonDoc.as<WifiNetwork>());
    // Serial.printf("Parsed network! SSid= %s, Pass = %s", network->getSSID(), network->getPassword());

    Serial.println("Parsing ssid and pass");
    const char* ssid = jsonDoc["SSID"].as<const char*>();
    const char* password = jsonDoc["Password"].as<const char*>();
    Serial.println("Parsed ssid and pass");
    Serial.println(ssid);
    Serial.println(password);

    // Memory is managed in repo
    WifiNetwork* network = new WifiNetwork(ssid, password);

    return network;
}

const char* JsonTokenService::serializeNetworksArray( NetworkRepo& networksInput)
{
    JsonDocument jsonDoc;
    JsonArray networks = jsonDoc.to<JsonArray>();
    for (int i = 0; i < networksInput.getCount(); i++) {

        WifiNetwork& network=networksInput[i];
        JsonObject networkObj = networks.add<JsonObject>();
        networkObj["ssid"] = network.getSSID();
        networkObj["password"] = network.getPassword();
    }

    const int BUFFER_SIZE = 500;
    char* json = new char[BUFFER_SIZE];
    serializeJson(jsonDoc, json, BUFFER_SIZE);
    Serial.println(json);
    return json;
}