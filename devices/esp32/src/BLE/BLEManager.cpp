#include <Arduino.h>
#include "BLEManager.h"

BLEManager::BLEManager(const char *deviceName, const char *serviceUUID, const char *characteristicUUID)
    : deviceName(deviceName), serviceUUID(serviceUUID), characteristicUUID(characteristicUUID) {}

void BLEManager::setupBLE()
{
    Serial.println("Initializing BLE...");
    BLEDevice::init(deviceName);                    // Initialize the device
    BLEServer *pServer = BLEDevice::createServer(); // Create a BLE server
    pServer->setCallbacks(this);                    // Set server callbacks

    BLEService *pService = pServer->createService(serviceUUID); // Create a BLE service
    pCharacteristic = pService->createCharacteristic(
        characteristicUUID,
        BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_BROADCAST | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_INDICATE | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR); // Create a characteristic

    pCharacteristic->setCallbacks(this); // Set characteristic callbacks
    pService->start();                   // Start the service

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising(); // Get the advertising object
    pAdvertising->addServiceUUID(serviceUUID);                  // Advertise the service UUID
    pAdvertising->setScanResponse(true);
    pAdvertising->start(); // Start advertising

    Serial.println("BLE Initialized and Advertising started");
}

void BLEManager::sendData(const uint8_t *data, size_t length)
{
    if (pCharacteristic != nullptr && data != nullptr && length > 0)
    {
        const size_t maxChunkSize = 600; // Maximum data size
        size_t offset = 0;

        while (offset < length)
        {
            size_t chunkSize = ((length - offset) > maxChunkSize) ? maxChunkSize : (length - offset);
            pCharacteristic->setValue(const_cast<uint8_t *>(data) + offset, chunkSize);
            pCharacteristic->notify();
            offset += chunkSize;
            delay(20); // Ensure the client has time to process the data
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
    BLEDevice::startAdvertising(); // Restart advertising on disconnect
}