#include <Arduino.h>
#include "BLE/BLEManager.h"

// Define your UUIDs and device name
#define DEVICE_NAME "EBL-ESP32"
#define SERVICE_UUID "aee41206-d70d-47f0-9a4f-b9f4faba2a28"
#define CHARACTERISTIC_UUID "dd83e3eb-324f-46a7-a79c-981c40f2401b"

BLEManager bleManager(DEVICE_NAME, SERVICE_UUID, CHARACTERISTIC_UUID);

void setup()
{
  Serial.begin(115200);
  delay(2000); // Short delay for serial connection to establish

  bleManager.setupBLE();
}

void loop()
{
  // Your loop code here, if needed
  delay(1000); // Example delay
}