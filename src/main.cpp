#include <Arduino.h>
#include <driver/i2s.h>

#define I2S_WS 15
#define I2S_SD 13
#define I2S_SCK 2
#define I2S_PORT I2S_NUM_0

// Function prototypes
void i2s_install();
void i2s_setpin();

void setup()
{
  Serial.begin(115200);
  Serial.println("Setup I2S ...");
  delay(1000);
  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);
  delay(500);
}

void loop()
{
  size_t bytes_read;
  int32_t sample = 0;
  // The number of bytes you want to read. For a 32-bit sample, this should be 4.
  const size_t num_bytes_to_read = sizeof(sample);
  // Call i2s_read with the correct arguments.
  esp_err_t result = i2s_read(I2S_PORT, &sample, num_bytes_to_read, &bytes_read, portMAX_DELAY);
  // Check if the read was successful and bytes were actually read.
  if (result == ESP_OK && bytes_read > 0)
  {
    Serial.println(sample);
  }
}

void i2s_install()
{
  const i2s_config_t i2s_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = 44100,
      .bits_per_sample = i2s_bits_per_sample_t(16),
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
      .intr_alloc_flags = 0, // default interrupt priority
      .dma_buf_count = 8,
      .dma_buf_len = 64,
      .use_apll = false};
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin()
{
  const i2s_pin_config_t pin_config = {
      .bck_io_num = I2S_SCK,
      .ws_io_num = I2S_WS,
      .data_out_num = -1,
      .data_in_num = I2S_SD};
  i2s_set_pin(I2S_PORT, &pin_config);
}