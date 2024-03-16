#include <Arduino.h>
#include "WiFiHandler.h"
#include "WebSocketHandler.h"
#include "AudioHandler.h"
#include "SleepHandler.h"
#include "TouchHandler.h"
#include <esp32-hal-touch.h>
#include <esp_sleep.h>

#define bufferLen 1024

#define TOUCH_PIN 2
#define LED_PIN 43

extern String receivedSSID;
extern String receivedPassword;
extern bool credentialsReceived;

int16_t sBuffer[bufferLen];

const char *ssid = "Early Bird Labs";
const char *password = "Hatchling22!";
const char *websocket_server_host = "192.168.4.142";
const uint16_t websocket_server_port = 8888;

void micTask(void *parameter);
void enterSleepMode();
void exitSleepMode();
// void touchCallback();

WiFiHandler wifiHandler(ssid, password);
WebSocketHandler webSocketHandler(websocket_server_host, websocket_server_port);
AudioHandler audioHandler(&webSocketHandler);
SleepHandler sleepHandler;
// TouchHandler touchHandler(TOUCH_PIN, touchCallback);

RTC_DATA_ATTR bool touchDetected = false;
RTC_DATA_ATTR bool isSleeping = false;

RTC_DATA_ATTR int threshold;

void powerOnCallback()
{
  touchDetected = true;
}

void setThreshold()
{
  int initialTouchValue = touchRead(TOUCH_PIN);
  threshold = initialTouchValue * 0.6;
  // threshold = 30000;
  Serial.print("Threshold set to: ");
  Serial.println(threshold);
}

void setup()
{

  Serial.begin(115200);
  delay(3000); // Shortened delay to speed up startup
  Serial.println("ESP32 Touch Interrupt Test");

  setThreshold();

  esp_sleep_enable_touchpad_wakeup();

  touchSleepWakeUpEnable(TOUCH_PIN, threshold);

  touchAttachInterrupt(TOUCH_PIN, powerOnCallback, threshold);

  wifiHandler.connect();
  printf("WiFiHandler connected\n");
  if (wifiHandler.isConnected())
  {
    webSocketHandler.connect();
  }
  xTaskCreatePinnedToCore(micTask, "micTask", 10000, NULL, 1, NULL, 1);
}

void loop()
{
  int touchValue = touchRead(TOUCH_PIN);
  printf("Touch value: %d\n", touchValue);

  if (touchValue < threshold)
  {
    enterSleepMode();
    isSleeping = true;
  }

  // if (touchDetected)
  // {
  //   printf("Touch detected in loop\n");
  //   printf("isSleeping: %d\n", isSleeping);
  //   if (!isSleeping)
  //   {
  //     enterSleepMode();
  //     isSleeping = true;
  //   }
  //   else
  //   {
  //     exitSleepMode();
  //     isSleeping = false;
  //   }
  //   touchDetected = false; // Reset the flag
  // }
  delay(500);
}

void enterSleepMode()
{
  // end mic task
  Serial.println("Disconnecting WebSocket...");
  webSocketHandler.disconnect(); // Assuming WebSocketHandler has a disconnect method

  Serial.println("Disconnecting WiFi...");
  wifiHandler.disconnect(); // Assuming WiFiHandler has a disconnect method

  Serial.println("Entering sleep mode...");
  Serial.end();
  esp_light_sleep_start();
  Serial.begin(115200);

  Serial.println("Waking up...");

  // Wake-up handling logic here
  exitSleepMode(); // Call exitSleepMode right after waking up
}

void exitSleepMode()
{
  Serial.println("Exiting sleep mode...");

  // Check the wake-up reason (optional, if specific handling is needed)
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason == ESP_SLEEP_WAKEUP_TOUCHPAD)
  {
    Serial.println("Woke up from touchpad");
  }

  Serial.println("Reconnecting WiFi...");
  wifiHandler.connect();
  while (!wifiHandler.isConnected())
  {
    delay(1000); // Wait for WiFi to connect
    Serial.println("Reconnecting to WiFi...");
  }

  Serial.println("Reconnecting WebSocket...");
  webSocketHandler.connect();
  while (!webSocketHandler.isConnected())
  {
    delay(1000); // Wait for WebSocket to connect
    Serial.println("Reconnecting to WebSocket...");
  }

  // Additional wake-up actions can be added here
}

void micTask(void *parameter)
{
  audioHandler.begin(); // Initialize the AudioHandler

  size_t bytesIn = 0; // Number of bytes read from the mic
  while (1)           // Loop forever
  {
    audioHandler.readMic(sBuffer, sizeof(sBuffer), bytesIn); // Use AudioHandler to read the mic
    if (bytesIn > 0 && webSocketHandler.isConnected())       // Check if the number of bytes read from the mic is greater than 0 and if the WebSocket is connected
    {
      // Check if the AudioHandler is in speaking mode
      if (audioHandler.getIsSpeaking()) // If the AudioHandler is in speaking mode
      {
        // If in speaking mode, send the buffer over WebSocket
        webSocketHandler.sendBinary((const char *)sBuffer, bytesIn);
      }
    }
  }
}