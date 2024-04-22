#ifndef BTCONTROLLER_H
#define BTCONTROLLER_H

#include "Models/WifiNetwork.h"
#include "Repos/NetworkRepo.h"
#include "Services/BleService.h"
#include "Services/IBleService.h"
#include "Services/JsonTokenService.h"
enum BtStatusCodes {
    UnknownErrorOccurred,
    GetNetworksSuccessfull,
    SentNetworksSuccessfully,
    AddedNetworkSuccessfully,
    RemovedNetworkSuccessfully,
    FailedToAddNetwork,
    FailedToRemoveNetwork,
    GetNetworksFailed
};
class BtController {
public:
    BtController(NetworkRepo& networks, BleService* ble);
    ~BtController();
    bool readCommand(); // Reads and processess command

private:
    bool addNetwork(WifiNetwork* network);

    bool deleteNetwork(WifiNetwork* network);
    void sendNetworks();
    NetworkRepo& networks;
    BleService* bt;
    JsonTokenService* jsonService;
    const char* templateString = "Code: %i; %s";
};
#endif