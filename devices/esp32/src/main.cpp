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
  threshold = initialTouchValue * 0.4;
  // threshold = 30000;
  Serial.print("Threshold set to: ");
  Serial.println(threshold);
}

void configureTouchWakeUp()
{
  esp_sleep_enable_touchpad_wakeup();
  touchSleepWakeUpEnable(TOUCH_PIN, threshold);
  touchAttachInterrupt(TOUCH_PIN, powerOnCallback, threshold);
}

void setup()
{

  Serial.begin(115200);
  delay(5000); // Shortened delay to speed up startup
  Serial.println("ESP32 Touch Interrupt Test");

  setThreshold();

  configureTouchWakeUp();

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
  Serial.println("Disconnecting WebSocket...");
  webSocketHandler.disconnect(); // Proper disconnection
  delay(100);                    // Short delay to ensure disconnection completes

  Serial.println("Disconnecting WiFi...");
  wifiHandler.disconnect(); // Proper disconnection
  delay(100);               // Short delay to ensure disconnection completes

  Serial.println("Entering sleep mode...");
  esp_light_sleep_start();

  exitSleepMode(); // Call exitSleepMode right after waking up
}

void exitSleepMode()
{
  Serial.begin(115200);
  Serial.println("Exiting sleep mode...");

  configureTouchWakeUp();

  // Reinitialize WiFi
  WiFi.disconnect(true);
  delay(1000); // Allow time for the WiFi hardware to initialize properly

  Serial.println("Reconnecting WiFi...");
  wifiHandler.connect();
  long wifiRetryDelay = 1000; // Start with a 1 second delay
  while (!wifiHandler.isConnected())
  {
    delay(wifiRetryDelay);
    Serial.println("Reconnecting to WiFi...");
    wifiHandler.connect();                                                   // Attempt to reconnect
    wifiRetryDelay = std::min(wifiRetryDelay * 2, static_cast<long>(60000)); // Exponential backoff to a maximum of 60 seconds
  }
  Serial.println("WiFi connected");

  // Reinitialize WebSocket
  webSocketHandler.disconnect(); // Ensure any existing connection is closed
  delay(100);                    // Short delay to ensure disconnection completes

  Serial.println("Reconnecting WebSocket...");
  webSocketHandler.connect();
  long wsRetryDelay = 1000; // Start with a 1 second delay
  while (!webSocketHandler.isConnected())
  {
    delay(wsRetryDelay);
    Serial.println("Reconnecting to WebSocket...");
    webSocketHandler.connect();                                          // Attempt to reconnect
    wsRetryDelay = std::min(wsRetryDelay * 2, static_cast<long>(60000)); // Exponential backoff to a maximum of 60 seconds
  }
  Serial.println("WebSocket connected");

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