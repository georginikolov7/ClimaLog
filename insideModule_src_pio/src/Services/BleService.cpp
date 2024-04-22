#include "Services/BleService.h"

void BleService::init(const char* deviceName)
{
    BLEDevice::init(deviceName);
    server = BLEDevice::createServer();
    service = server->createService(SERVICE_UUID);
    readCharacteristic = service->createCharacteristic(READ_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);
    writeCharacteristic = service->createCharacteristic(WRITE_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);
    service->start();
    BLEAdvertising* pAdvertising = server->getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    pAdvertising->start();
}
void BleService::deinit()
{
    BLEDevice::deinit(true);
}
void BleService::read(char* buffer, int size)
{
    if (isAvailable()) {
        strncpy(buffer, readCharacteristic->getValue().c_str(), size);
        readCharacteristic->setValue("");
    }
}
void BleService::send(const char* msg, int length)
{
    char buffer[PACKET_SIZE + 2]; // 2 extra bytes for \n and \0
    int j = 0;
    for (int i = 0; i < length; i++) {
        buffer[j] = msg[i];
        j++;
        if (j == PACKET_SIZE) {
            buffer[j] = '\0';
            writeCharacteristic->setValue(buffer);
            writeCharacteristic->notify();
            while (!isAvailable()) {
            }
            if (readCharacteristic->getValue() == "readOK") {
                j = 0;
                readCharacteristic->setValue("");
            } else {
                Serial.println("Error occured while sending msg");
                writeCharacteristic->setValue("Error");
            }
        }
        delay(100);
    }
    // terminate end of msg:
    buffer[j] = '\n';
    buffer[j + 1] = '\0';

    writeCharacteristic->setValue(buffer);
    writeCharacteristic->notify();

    while (!isAvailable()) {
    }
    if (readCharacteristic->getValue() == "readOK") {
        readCharacteristic->setValue("");
        Serial.printf("Set value to %s\n", msg);
    } else {
        Serial.println("Error occurred while sending command");
    }
}
bool BleService::isAvailable()
{
    return readCharacteristic->getValue() != "";
}
bool BleService::isConnected()
{
    return server->getConnectedCount() == 1;
}