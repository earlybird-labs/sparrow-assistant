#include <Arduino.h>
#include "SDCardManager.h"

SDCardManager::SDCardManager() {
    // Constructor implementation (if needed)
}

bool SDCardManager::initSDCard() {
    if (!SD.begin()) {
        Serial.println("SD Card initialization failed!");
        return false;
    }
    Serial.println("SD Card initialized.");
    return true;
}

bool SDCardManager::sendDataInChunks(const String& fileName, const uint8_t* data, size_t length) {
    File file = SD.open(fileName, FILE_WRITE);
    if (!file) {
        Serial.println("Opening file failed");
        return false;
    }

    size_t written = 0;
    while (written < length) {
        size_t toWrite = min(length - written, static_cast<size_t>(512)); // Ensure both arguments are size_t
        size_t didWrite = file.write(data + written, toWrite);
        if (didWrite == 0) {
            Serial.println("Writing to file failed");
            file.close();
            return false;
        }
        written += didWrite;
    }

    file.close();
    Serial.println("Data written in chunks successfully.");
    return true;
}