#include <Arduino.h>
#include "BLEManager.h"

BLEManager::BLEManager(const char *deviceName, const char *serviceUUID, const char *characteristicUUID)
    : deviceName(deviceName), serviceUUID(serviceUUID), characteristicUUID(characteristicUUID) {}

void BLEManager::setupBLE()
{
    BLEDevice::init(deviceName);
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(this);

    // Request a larger MTU size for efficient audio data transmission
    BLEDevice::setMTU(517);

    BLEService *pService = pServer->createService(serviceUUID);
    pCharacteristic = pService->createCharacteristic(
        characteristicUUID,
        BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE_NR); // Adjusted property name

    pCharacteristic->setCallbacks(this);
    pService->start();

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
}

void BLEManager::sendData(const uint8_t *data, size_t length)
{
    if (pCharacteristic != nullptr && data != nullptr && length > 0)
    {
        const size_t maxChunkSize = 600; // Maximum data size
        size_t offset = 0;

        while (offset < length)
        {
            // Calculate chunk size
            size_t chunkSize = ((length - offset) > maxChunkSize) ? maxChunkSize : (length - offset);

            // Set value to the next chunk of data
            pCharacteristic->setValue(const_cast<uint8_t *>(data) + offset, chunkSize);
            pCharacteristic->notify(); // Notify the client

            offset += chunkSize; // Move to the next chunk
            delay(20);           // Optional: delay to ensure the client has time to process the data
        }
    }
}

void BLEManager::onWrite(BLECharacteristic *pCharacteristic)
{
    std::string value = pCharacteristic->getValue();
    if (!value.empty())
    {
        Serial.println("*********");
        Serial.print("New value: ");
        Serial.println(value.c_str());
        Serial.println("*********");
    }
}

void BLEManager::sendStartMessage()
{
    pCharacteristic->setValue("START");
    pCharacteristic->notify();
}

void BLEManager::sendEndMessage()
{
    pCharacteristic->setValue("END");
    pCharacteristic->notify();
}

void BLEManager::onConnect(BLEServer *pServer)
{
    Serial.println("Device connected");
}

void BLEManager::onDisconnect(BLEServer *pServer)
{
    Serial.println("Device disconnected");
    // Optionally restart advertising on disconnect
    BLEDevice::startAdvertising();
}