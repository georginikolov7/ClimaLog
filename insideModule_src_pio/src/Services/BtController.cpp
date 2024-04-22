#include "Services/BtController.h"
BtController::BtController(NetworkRepo& networks, BleService* ble)
    : networks(networks)
{
    this->bt = ble;
    this->jsonService = new JsonTokenService;
}
BtController::~BtController()
{
    delete jsonService;
}

// returns false if command is invalid
bool BtController::readCommand()
{
    // sendNetworks();
    char input[1000];
    bt->read(input, sizeof(input));
    Serial.println(input);
    if (input[0] == '\0') {
        // string is null => throw error
        Serial.println(F("Input command is null!!!"));
        return false;
    } else {
        // Structure: "commandName; {Tokens as json}"
        char* command;
        command = strtok(input, "; ");
        if (command == NULL) {
            Serial.println(F("Incorrect command syntax!!!"));
            return false;
        }
        Serial.printf("Command: %s\n", command);
        char* rest = strtok(NULL, "; ");
        if (rest == NULL) {
            Serial.println(F("Incorrect token!!!"));
            return false;
        }
        Serial.printf("Rest: %s\n", rest);

        // Read the command:
        if (strcmp(command, "getNetworks") == 0) {
            sendNetworks();
        } else if (strcmp(command, "addNetwork") == 0) {
            Serial.println("Adding network...");
            WifiNetwork* network = jsonService->parseNetwork(rest);
            char msg[100];
            if (addNetwork(network)) {
                Serial.printf("Current networks count: %i\n", networks.getCount());
                Serial.println(F("Network successfully added to repo"));
                snprintf(msg, sizeof(msg), templateString, BtStatusCodes::AddedNetworkSuccessfully, "0");
            } else {
                Serial.println(F("Error occurred! Could not add network"));
                snprintf(msg, sizeof(msg), templateString, BtStatusCodes::FailedToAddNetwork, "0");
            }
            bt->send(msg, strlen(msg));
        } else if (strcmp(command, "deleteNetwork") == 0) {
            Serial.println(F("Deleting network..."));
            WifiNetwork* network = jsonService->parseNetwork(rest);
            char msg[100];
            if (deleteNetwork(network)) {
                Serial.println(F("Network successfully deleted from repo"));

                snprintf(msg, sizeof(msg), templateString, BtStatusCodes::RemovedNetworkSuccessfully, "0");
            } else {
                Serial.println(F("Error occurred! Could not delete network"));

                snprintf(msg, sizeof(msg), templateString, BtStatusCodes::FailedToRemoveNetwork, "0");
            }
            bt->send(msg, strlen(msg));
        }
        return true;
    }
}

bool BtController::addNetwork(WifiNetwork* network)
{
    if (network == nullptr) {
        Serial.println(F("Error! Network is nullptr!"));
        return false;
    }
    StatusCode code = networks.add(*network);
    if (code != StatusCode::NoError) {
        Serial.println(F("Error occured! Could not add network!"));
        return false;
    }
    return true;
}

bool BtController::deleteNetwork(WifiNetwork* network)
{
    if (network == nullptr) {
        Serial.println(F("Error! Network is nullptr!"));
        return false;
    }
    StatusCode code = networks.remove(*network);
    if (code != StatusCode::NoError) {
        Serial.println(F("Error occured! Could not delete network!"));
        return false;
    }
    return true;
}
void BtController::sendNetworks()
{
    WifiNetwork& netRef = networks[0];
    const char* networksJSON = jsonService->serializeNetworksArray(networks);

    BtStatusCodes code;
    // char codeString[15];
    if (networksJSON != nullptr) {
        code = BtStatusCodes::GetNetworksSuccessfull;
    } else {
        code = BtStatusCodes::GetNetworksFailed;
        Serial.println(F("Error occured! Could not serialize networks array"));
    }
    char msg[1000];
    msg[0] = '\0';
    snprintf(msg, sizeof(msg), templateString, code, networksJSON);
    Serial.println(F("Sending networks"));
    Serial.println(msg);
    bt->send(msg, strlen(msg));

    delete[] networksJSON;
}
