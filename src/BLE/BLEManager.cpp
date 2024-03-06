#include <Arduino.h>
#include "BLEManager.h"

BLEManager::BLEManager(const char *deviceName, const char *serviceUUID, const char *characteristicUUID)
    : deviceName(deviceName), serviceUUID(serviceUUID), characteristicUUID(characteristicUUID) {}

void BLEManager::setupBLE()
{
    BLEDevice::init(deviceName);
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(this);

    BLEService *pService = pServer->createService(serviceUUID);
    pCharacteristic = pService->createCharacteristic(
        characteristicUUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

    pCharacteristic->setCallbacks(this);
    pCharacteristic->setValue("Hello World");
    pService->start();

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
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