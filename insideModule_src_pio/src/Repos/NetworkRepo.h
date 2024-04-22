#ifndef NETWORKREPO_H
#define NETWORKREPO_H
#include "Arduino.h"
#include "Models/WifiNetwork.h"
#include "Repos/IRepo.h"
#include "StatusCodes.h"
class NetworkRepo : public IRepo<WifiNetwork> {
public:
    ~NetworkRepo();

    NetworkRepo(int maxSize);

    StatusCode add(WifiNetwork& network) override;

    StatusCode remove(WifiNetwork& refNetwork) override;

    const int getCount() const override;

    const WifiNetwork* getElements() const override;

    WifiNetwork& operator[](size_t index) const override;

private:
    WifiNetwork* networks;
    int maxSize = 0;
    int count = 0;
    const int DEFAULT_MAX_LENGTH = 10;
    // if element exists return its index else return -1
    int getIndex(WifiNetwork& network);

    // startIndex: index of removed element
    void shiftLeft(int startIndex);
};
#endif