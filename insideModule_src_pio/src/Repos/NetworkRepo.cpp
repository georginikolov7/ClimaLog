#include "Repos/NetworkRepo.h"
NetworkRepo::~NetworkRepo()
{
    delete[] networks;
}
NetworkRepo::NetworkRepo(int maxSize)
{
     if (maxSize <= 0) {
        Serial.println("Size cannot be negative!");
        this->maxSize=DEFAULT_MAX_LENGTH;
    }else {
        this->maxSize = maxSize;
        // Instantialize array:
        networks = new WifiNetwork[maxSize];
    }
}
StatusCode NetworkRepo::add(WifiNetwork& network) 
{
    if (getIndex(network) != -1) {
        Serial.println("Item already added to array");
        return StatusCode::ItemAlreadyAdded;
    }
    if (count == maxSize) {
        Serial.println("Repo is full!");
        return StatusCode::RepoFull;
    }
    // Save the new network:
    networks[count] = network;
    // Increment the count:
    count++;
    return StatusCode::NoError;
}
StatusCode NetworkRepo::remove(WifiNetwork& refNetwork) 
{
    int elementIndex = getIndex(refNetwork);
    if (elementIndex != -1) {
        shiftLeft(elementIndex);
        count--;
        return StatusCode::NoError;
        ;
    } else {
        // error element does not exist:
        Serial.println("Element you are trying to delete does not exist in Repo!");
        return StatusCode::ItemIsNotAdded;
    }
}
const int NetworkRepo::getCount() const 
{
    return count;
}
const WifiNetwork* NetworkRepo::getElements() const 
{
    return networks;
}
WifiNetwork& NetworkRepo::operator[](size_t index) const 
{
    static WifiNetwork dummy;
    if (index < 0 || index >= count) {
        Serial.println("Index out of range");
        return dummy;
    }
    return networks[index];
}

int NetworkRepo::getIndex(WifiNetwork& network)
{
    int index = -1;
    for (int i = 0; i < count; i++) {
        if (networks[i] == network) {
            index = i;
        }
    }
    return index;
}

void NetworkRepo::shiftLeft(int startIndex)
{
    // check startIndex:
    if (startIndex < 0 || startIndex >= count) {
        Serial.println("Index out of range!");
        return;
    }

    for (int i = startIndex + 1; i < count; i++) {
        networks[i - 1] = networks[i];
    }

    // Clear the last element:
    networks[count - 1] = WifiNetwork();
}