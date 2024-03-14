#include <Arduino.h>
#include "BLEManager.h"

// Variables to store WiFi credentials received via BLE
static String receivedSSID = "";
static String receivedPassword = "";
static bool credentialsReceived = false;

// Constructor for BLEManager class, initializing with device name, service UUID, and characteristic UUID
BLEManager::BLEManager(const char *deviceName, const char *serviceUUID, const char *characteristicUUID)
    : deviceName(deviceName), serviceUUID(serviceUUID), characteristicUUID(characteristicUUID) {}

// Setup BLE environment, including server, service, and characteristic
void BLEManager::setupBLE()
{
    Serial.println("Initializing BLE...");
    BLEDevice::init(deviceName);                    // Initialize the BLE device with the given name
    BLEServer *pServer = BLEDevice::createServer(); // Create a new BLE server
    pServer->setCallbacks(this);                    // Set this class as the server's callback handler

    // Create a BLE service with the specified UUID
    BLEService *pService = pServer->createService(serviceUUID);
    // Create a BLE characteristic with various properties
    pCharacteristic = pService->createCharacteristic(
        characteristicUUID,
        BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_BROADCAST | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_INDICATE | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);

    pCharacteristic->setCallbacks(this); // Set this class as the characteristic's callback handler
    pService->start();                   // Start the BLE service

    // Setup BLE advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(serviceUUID); // Advertise this service's UUID
    pAdvertising->setScanResponse(true);
    pAdvertising->start(); // Start advertising

    Serial.println("BLE Initialized and Advertising started");
}

// Callback function for when a BLE characteristic is written to
void BLEManager::onWrite(BLECharacteristic *pCharacteristic)
{
    std::string value = pCharacteristic->getValue(); // Get the value written to the characteristic
    if (!value.empty())
    {
        // Parse the received data assuming it's in the format "SSID:PASSWORD"
        String receivedData = String(value.c_str());
        int delimiterPos = receivedData.indexOf(':'); // Find the position of the delimiter
        if (delimiterPos != -1)                       // Check if the delimiter was found
        {
            // Split the received data into SSID and password
            receivedSSID = receivedData.substring(0, delimiterPos);
            receivedPassword = receivedData.substring(delimiterPos + 1);
            credentialsReceived = true; // Mark that credentials have been received

            // Log the received WiFi credentials
            Serial.println("Received WiFi credentials");
            Serial.print("SSID: ");
            Serial.println(receivedSSID);
            Serial.print("Password: ");
            Serial.println(receivedPassword);
        }
    }
}

// Function to send data over BLE
void BLEManager::sendData(const uint8_t *data, size_t length)
{
    if (pCharacteristic != nullptr && data != nullptr && length > 0)
    {
        const size_t maxChunkSize = 600; // Define the maximum size of data chunks
        size_t offset = 0;
        // This loop sends data in chunks to manage large data transfers over BLE efficiently.
        while (offset < length) // Continue until all data is sent
        {
            // Calculate chunk size. It's either the maxChunkSize or the remainder if less than maxChunkSize.
            size_t chunkSize = ((length - offset) > maxChunkSize) ? maxChunkSize : (length - offset);
            // Set the value of the characteristic to the current chunk of data.
            pCharacteristic->setValue(const_cast<uint8_t *>(data) + offset, chunkSize);
            // Notify the client that the characteristic's value has been updated.
            pCharacteristic->notify();
            // Move the offset forward by the chunk size for the next iteration.
            offset += chunkSize;
            // Introduce a short delay to ensure the client has time to process the data chunk.
            delay(20);
        }
    }
}

// Sends a predefined "START" message to indicate the beginning of a data transfer.
void BLEManager::sendStartMessage()
{
    // Set the characteristic's value to "START".
    pCharacteristic->setValue("START");
    // Notify connected clients that the value has been updated.
    pCharacteristic->notify();
}

// Sends a predefined "END" message to indicate the end of a data transfer.
void BLEManager::sendEndMessage()
{
    // Set the characteristic's value to "END".
    pCharacteristic->setValue("END");
    // Notify connected clients that the value has been updated.
    pCharacteristic->notify();
}

// Callback function that is called when a device connects to the BLE server.
void BLEManager::onConnect(BLEServer *pServer)
{
    // Log the connection event.
    Serial.println("Device connected");
}

void BLEManager::stopBLE()
{
    BLEDevice::deinit(false); // Deinitialize BLE without erasing services
    Serial.println("BLE Stopped.");
}

// Callback function that is called when a device disconnects from the BLE server.
void BLEManager::onDisconnect(BLEServer *pServer)
{
    // Log the disconnection event.
    Serial.println("Device disconnected");
    // Restart advertising to allow new devices to connect.
    BLEDevice::startAdvertising();
}