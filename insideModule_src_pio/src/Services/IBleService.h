#ifndef I_BLE_SERVICE_H
#define I_BLE_SERVICE_H

class IBleService {
public:
    virtual void read(char* buffer, int size) = 0;
    virtual void send(const char* msg, int length) = 0;
    virtual bool isAvailable() = 0;
    virtual bool isConnected() = 0;
};
#endif