#ifndef SPIFFSMANAGER_H
#define SPIFFSMANAGER_H

#include "Arduino.h"
#include "ISpiffsManager.h"
#include "SPIFFS.h"
class SpiffsManager : public ISpiffsManager {
    bool initSpiffs() override;
    bool readFile(const char* path, String& output) override;
    bool writeToFile(const char* path, const char* msg) override;
};
#endif