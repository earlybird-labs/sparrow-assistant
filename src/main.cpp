#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>


#define SERVICE_UUID        "aee41206-d70d-47f0-9a4f-b9f4faba2a28"
#define CHARACTERISTIC_UUID "dd83e3eb-324f-46a7-a79c-981c40f2401b"

// Callback class for BLE characteristic events
class MyCallbacks: public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic) override
  {
    std::string value = pCharacteristic->getValue();

    if (!value.empty())
    {
      Serial.println("*********");
      Serial.print("New value: ");
      Serial.println(value.c_str());
      Serial.println("*********");

      // Append "RETURNED" to the received value and update the characteristic
      std::string response = value + "RETURNED";
      pCharacteristic->setValue(response);
      pCharacteristic->notify(); // Notify connected client about the update
    }
  }
};

void setup()
{
  Serial.begin(115200);
  Serial.println("Follow these steps:");
  Serial.println("1- Download and install a BLE scanner app on your phone");
  Serial.println("2- Scan for BLE devices in the app");
  Serial.println("3- Connect to EARLYBIRD-ESP32-BLE-Server");
  Serial.println("4- Write something to the CUSTOM CHARACTERISTIC in CUSTOM SERVICE");
  Serial.println("5- Observe the response");

  // Initialize BLE device and create server
  BLEDevice::init("EARLYBIRD-ESP32-BLE-Server");
  BLEServer *pServer = BLEDevice::createServer();

  // Create service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create characteristic with read and write properties
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  // Set callback for characteristic
  pCharacteristic->setCallbacks(new MyCallbacks());

  // Set initial value for characteristic
  pCharacteristic->setValue("Hello World");
  pService->start(); // Start the service

  // Start advertising
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

void loop()
{
  delay(2000); // Main loop does nothing, BLE operations are event-driven
}