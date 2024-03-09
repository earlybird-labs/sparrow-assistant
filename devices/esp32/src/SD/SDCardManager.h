#ifndef SDCARDMANAGER_H
#define SDCARDMANAGER_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

class SDCardManager {
public:
    SDCardManager(); // Constructor
    bool initSDCard(); // Initializes the SD card
    bool sendDataInChunks(const String& fileName, const uint8_t* data, size_t length); // Sends data in chunks to a file

private:
    // Add any private member variables here if needed
};

#endif // SDCARDMANAGER_H