#ifndef JSON_TOKEN_SERVICE
#define JSON_TOKEN_SERVICE
#include "ArduinoJson.h"
#include "Models/WifiNetwork.h"
#include "Repos/NetworkRepo.h"
class JsonTokenService {
public:
    JsonTokenService() = default;
    ~JsonTokenService() = default;

    WifiNetwork* parseNetwork(const char* json);
    const char* serializeNetworksArray(NetworkRepo& networks);
};
#endif