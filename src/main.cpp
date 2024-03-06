#include <Arduino.h>
// Include AudioManager and BLEManager headers
#include "Audio/AudioManager.h"
#include "BLE/BLEManager.h"

// Define DEVICE_NAME, SERVICE_UUID, and CHARACTERISTIC_UUID
const char *DEVICE_NAME = "EBL-CANARY";
const char *SERVICE_UUID = "aaef8ad8-7bff-4351-b04a-391d5fd66bc7";
const char *CHARACTERISTIC_UUID = "60abdc93-8293-49f2-918b-ad52250201d9";

AudioManager audioManager;
BLEManager bleManager(DEVICE_NAME, SERVICE_UUID, CHARACTERISTIC_UUID);

void setup()
{
  Serial.begin(115200);

  audioManager.begin();
  bleManager.setupBLE();
  Serial.println("Setup complete, starting loop...");
}

void loop()
{
  try
  {
    Serial.println("Starting audio recording for 5 seconds...");
    audioManager.recordAudio();
    delay(5000); // Record for 5 seconds
    if (!audioManager.isRecording())
    {
      Serial.println("Audio recording was not active.");
      return;
    }

    Serial.println("Stopping audio recording...");
    audioManager.stopRecording();
    Serial.println("Preparing audio data for streaming...");
    auto audioData = audioManager.getAudioData(); // Retrieve audio data

    // Function to handle the streaming of audio data
    auto streamAudioData = [](const std::vector<uint8_t> &data)
    {
      const size_t maxBLEPayloadSize = 600;
      if (data.size() <= maxBLEPayloadSize)
      {
        Serial.println("Streaming audio data...");
        bleManager.sendData(data);
      }
      else
      {
        Serial.println("Audio data size exceeds BLE maximum payload size, splitting data...");
        for (size_t i = 0; i < data.size(); i += maxBLEPayloadSize)
        {
          auto end = std::min(data.size(), i + maxBLEPayloadSize);
          std::vector<uint8_t> chunk(data.begin() + i, data.begin() + end);
          Serial.println("Streaming audio data chunk...");
          bleManager.sendData(chunk);
        }
      }
    };

    // Stream the audio data
    streamAudioData(audioData);

    delay(1000); // Wait for a second before next recording
  }
  catch (const std::exception &e)
  {
    Serial.print("Error in loop: ");
    Serial.println(e.what());
  }
}