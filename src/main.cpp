#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID "aee41206-d70d-47f0-9a4f-b9f4faba2a28"
#define CHARACTERISTIC_UUID "dd83e3eb-324f-46a7-a79c-981c40f2401b"

// Define the pin where the microphone's analog output is connected
const int micAnalogPin = 39; // GPIO34 for analog input, choose according to your ESP32 board

// Declare pCharacteristic globally to maintain state across function calls
BLECharacteristic *pCharacteristic = nullptr;

// Custom callback class for handling BLE events
class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic) override
  {
    std::string value = pCharacteristic->getValue();

    // Only proceed if the value is not empty
    if (!value.empty())
    {
      // Log the received value to the Serial monitor
      Serial.println("*********");
      Serial.print("New value: ");
      Serial.println(value.c_str());
      Serial.println("*********");

      // Update the characteristic value with the received message
      pCharacteristic->setValue(value);
    }
  }
};

void setup()
{
  Serial.begin(115200);
  // Short delay for serial connection to establish
  delay(2000);

  // Initialize BLE device
  BLEDevice::init("EBL-ESP32");
  BLEServer *pServer = BLEDevice::createServer();

  // Create a BLE service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE characteristic
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  // Set the initial value for the characteristic
  pCharacteristic->setValue("Hello World");

  // Register callbacks for characteristic
  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  // Initialize the analog pin for microphone input
  pinMode(micAnalogPin, INPUT);
}

void loop()
{
  // Read the value from the microphone's analog output
  int micAnalogValue = analogRead(micAnalogPin);

  // Print the value to the Serial monitor
  Serial.print("Microphone (analog) value: ");
  Serial.println(micAnalogValue);

  // No need to update the characteristic value here as it's handled by the callback
  delay(50); // Shorter delay for more frequent readings
}
