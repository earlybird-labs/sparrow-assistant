#include <Arduino.h>
// Include AudioManager and BLEManager headers
#include "Audio/AudioManager.h"
#include "BLE/BLEManager.h"

// Assuming button is connected to pin 0
const int buttonPin = 0;
bool lastButtonState = LOW;

// Define DEVICE_NAME, SERVICE_UUID, and CHARACTERISTIC_UUID
const char *DEVICE_NAME = "EBL-CANARY";
const char *SERVICE_UUID = "aaef8ad8-7bff-4351-b04a-391d5fd66bc7";
const char *CHARACTERISTIC_UUID = "60abdc93-8293-49f2-918b-ad52250201d9";

AudioManager audioManager;
BLEManager bleManager(DEVICE_NAME, SERVICE_UUID, CHARACTERISTIC_UUID);

void setup()
{
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);

  audioManager.begin();
  bleManager.setupBLE();
}

void loop()
{
  try
  {
    bool currentButtonState = digitalRead(buttonPin);
    if (currentButtonState != lastButtonState)
    {
      Serial.print("Button state changed: ");
      Serial.println(currentButtonState ? "HIGH" : "LOW");

      if (currentButtonState == HIGH)
      {
        if (!audioManager.isRecording())
        {
          Serial.println("Starting audio recording...");
          audioManager.recordAudio();
        }
        else
        {
          Serial.println("Stopping audio recording and sending data...");
          audioManager.stopRecording();
          bleManager.sendData(audioManager.getAudioData());
        }
      }
      lastButtonState = currentButtonState;
    }
    delay(50); // Debounce delay
  }
  catch (const std::exception &e)
  {
    Serial.print("Error in loop: ");
    Serial.println(e.what());
  }
}