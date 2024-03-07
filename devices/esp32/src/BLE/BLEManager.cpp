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
        // Ensure the data and length are valid
        pCharacteristic->setValue(const_cast<uint8_t *>(data), length);
        pCharacteristic->notify(); // Efficiently stream the data
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