#include "SpiffsManager.h"

bool SpiffsManager::initSpiffs()
{
    if (!SPIFFS.begin(true)) {
        Serial.println("An error has occurred while mounting SPIFFS");
        return false;
    }
    Serial.println("SPIFFS mounted successfully");
    return true;
}

bool SpiffsManager::readFile(const char* path, String& output)
{
    Serial.printf("Reading file: %s\r\n", path);

    File file = SPIFFS.open(path);
    if (!file || file.isDirectory()) {
        Serial.println("- failed to open file for reading");
        return false;
    }
    Serial.println("- read from file:");
    while (file.available()) {
        output = file.readStringUntil('\n');
        Serial.println(output);
    }
    file.close();
    return true;
}
bool SpiffsManager::writeToFile(const char* path, const char* msg)
{
    Serial.printf("Writing file: %s\r\n", path);

    File file = SPIFFS.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("- failed to open file for writing");
        return false;
    }
    if (file.print(msg)) {
        Serial.println("- file written");
        file.close();
        return true;
    }
    Serial.println("- write failed");
    file.close();
    return false;
}