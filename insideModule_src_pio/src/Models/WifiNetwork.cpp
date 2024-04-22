#include "WifiNetwork.h"

WifiNetwork::WifiNetwork(const char* SSID, const char* pass)
{
    this->ssid = new char[strlen(SSID) + 1];
    ssid[0] = '\0';
    strcpy(ssid, SSID);

    this->password = new char[strlen(pass) + 1];
    password[0] = '\0';
    strcpy(password, pass);
}
WifiNetwork::~WifiNetwork()
{
    delete[] ssid;
    delete[] password;
}
bool WifiNetwork::operator==(WifiNetwork& other)
{
    return (strcmp(this->ssid, other.getSSID()) == 0 && strcmp(this->password, other.getPassword()) == 0);
}
const char* WifiNetwork::getSSID() const
{
    return ssid;
}
const char* WifiNetwork::getPassword() const
{
    return password;
}