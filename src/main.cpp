#include <Arduino.h>
#include "BluetoothSerial.h" // Header file for Bluetooth

// Check if BluetoothSerial library is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial ESP_BT; // Object for Bluetooth

void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(921600);
  Serial.println("Hello World!");

  // Bluetooth device name
  ESP_BT.begin("ESP32_BT_Test"); // Init Bluetooth with device name "ESP32_BT_Test"
  Serial.println("Bluetooth device is ready to pair");
}

void loop() {
  delay(1000);
  digitalWrite(2, HIGH);
  // Check if we receive anything from Bluetooth
  if (ESP_BT.available()) {
    String msg = ESP_BT.readString(); // Read what we receive
    Serial.println(msg); // Print it out
    ESP_BT.println("Received: " + msg); // Echo back to Bluetooth
  }
  delay(5000);
  digitalWrite(2, LOW);
}