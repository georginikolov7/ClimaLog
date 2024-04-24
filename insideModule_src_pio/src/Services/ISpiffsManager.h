#ifndef ISPIFFSMANAGER_h
#define ISPIFFSMANAGER_h
#include "Arduino.h"
class ISpiffsManager {
public:
    virtual bool initSpiffs() = 0;
    virtual bool readFile(const char* path, String& output) = 0;
    virtual bool writeToFile(const char* path, const char* msg) = 0;
};
#endif
