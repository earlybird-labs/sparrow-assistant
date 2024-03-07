#include <Arduino.h>
#include <driver/i2s.h>

#include "BLE/BLEManager.h"

#define I2S_WS 15
#define I2S_SD 13
#define I2S_SCK 2
#define I2S_PORT I2S_NUM_0

#define bufferCnt 10
#define bufferLen 1024
int16_t sBuffer[bufferLen];

BLEManager bleManager("ESP32", "4fafc201-1fb5-459e-8fcc-c5c9c331914b", "beb5483e-36e1-4688-b7f5-ea07361b26a8");

void i2s_install()
{
  // Set up I2S Processor configuration
  const i2s_config_t i2s_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = 44100,
      .bits_per_sample = i2s_bits_per_sample_t(16),
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
      .intr_alloc_flags = 0,
      .dma_buf_count = bufferCnt,
      .dma_buf_len = bufferLen,
      .use_apll = false};

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin()
{
  // Set I2S pin configuration
  const i2s_pin_config_t pin_config = {
      .bck_io_num = I2S_SCK,
      .ws_io_num = I2S_WS,
      .data_out_num = -1,
      .data_in_num = I2S_SD};

  i2s_set_pin(I2S_PORT, &pin_config);
}

// Forward declaration
void micTask(void *parameter);

void setup()
{
  Serial.begin(115200);

  bleManager.setupBLE();
  xTaskCreatePinnedToCore(micTask, "micTask", 10000, NULL, 1, NULL, 1);
}

void loop()
{
}

void micTask(void *parameter)
{
  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);

  size_t bytesIn = 0;
  const float gainFactor = 1.0;   // Example gain factor; adjust as needed
  const int16_t threshold = 2000; // Define your threshold here
  const uint32_t holdTime = 5000; // Continue transmitting for 5 seconds after signal drops
  uint32_t lastAboveThresholdTime = 0;

  while (1)
  {
    esp_err_t result = i2s_read(I2S_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);
    if (result == ESP_OK)
    {
      int64_t sum = 0;
      for (int i = 0; i < bytesIn / sizeof(int16_t); i++)
      {
        int32_t temp = sBuffer[i] * gainFactor;
        temp = (temp > INT16_MAX) ? INT16_MAX : (temp < INT16_MIN) ? INT16_MIN
                                                                   : temp;
        sBuffer[i] = temp;
        sum += abs(temp);
      }
      int16_t average = sum / (bytesIn / sizeof(int16_t));

      // Log the average signal level
      Serial.print("Average signal level: ");
      Serial.println(average);

      // Check if the average signal is above the threshold or if we're within the hold time
      uint32_t currentTime = millis();
      if (average > threshold)
      {
        lastAboveThresholdTime = currentTime;
      }

      if ((currentTime - lastAboveThresholdTime) <= holdTime)
      {
        // Send audio buffer over BLE
        bleManager.sendData((const uint8_t *)sBuffer, bytesIn);
      }
    }
  }
}