#ifndef BLE_SERVICE_H
#define BLE_SERVICE_H
#include "Arduino.h"
#include "Services/IBleService.h"
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <string>
#define SERVICE_UUID "594dc290-3914-4553-88b5-d8920f7511fb"
#define READ_CHARACTERISTIC_UUID "92e82d12-6b49-4add-a58d-d0c49895abd0"
#define WRITE_CHARACTERISTIC_UUID "7e4f9c4d-9a0d-47af-9a9a-25071a223f5d"

class BleService : public IBleService {
public:
    ~BleService()
    {
        BLEDevice::deinit();
        delete readCharacteristic;
        delete writeCharacteristic;
        delete server;
        delete service;
    }
    void init(const char* deviceName);
    void deinit();
    void read(char* buffer, int size) override;
    void send(const char* msg, int length) override;
    bool isAvailable() override;
    bool isConnected() override;
    void testCheck()
    {
        Serial.println("Hallo");
        Serial.println(service->getUUID().toString().c_str());
        Serial.println(readCharacteristic->getUUID().toString().c_str());
    }

private:
    BLEServer* server;
    BLEService* service;
    BLECharacteristic* readCharacteristic;
    BLECharacteristic* writeCharacteristic;
    bool deviceConnected = false;
    const int PACKET_SIZE = 18;
};
#endif