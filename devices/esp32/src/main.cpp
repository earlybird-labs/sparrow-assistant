#include <Arduino.h>
#include "WiFiHandler.h"
#include "WebSocketHandler.h"
#include "AudioHandler.h"
#include "SleepHandler.h"
#include "TouchHandler.h"
#include <esp32-hal-touch.h>

#define bufferLen 1024

#define TOUCH_PIN 2

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
void touchCallback();

WiFiHandler wifiHandler(ssid, password);
WebSocketHandler webSocketHandler(websocket_server_host, websocket_server_port);
AudioHandler audioHandler(&webSocketHandler);
SleepHandler sleepHandler;
TouchHandler touchHandler(TOUCH_PIN, touchCallback);

RTC_DATA_ATTR bool touchDetected = false;
RTC_DATA_ATTR bool isSleeping = false;

void touchCallback()
{
  touchDetected = true;
}

void setup()
{

  Serial.begin(115200);
  delay(3000); // Shortened delay to speed up startup
  Serial.println("ESP32 Touch Interrupt Test");

  int initialTouchValue = touchRead(TOUCH_PIN);
  int threshold = initialTouchValue * 0.60;

  touchAttachInterrupt(TOUCH_PIN, touchCallback, threshold);
  esp_sleep_enable_touchpad_wakeup();

  // Dynamically set the threshold based on initial touch value
}

void loop()
{
  printf("Looping...\n");
  if (touchDetected)
  {
    printf("Touch detected in loop\n");
    printf("isSleeping: %d\n", isSleeping);
    if (!isSleeping)
    {
      enterSleepMode();
      isSleeping = true;
    }
    else
    {
      isSleeping = false;
    }
    touchDetected = false; // Reset the flag
  }
  delay(500);
}

void enterSleepMode()
{
  Serial.println("Entering sleep mode...");
  esp_light_sleep_start();
  // Additional sleep mode entry actions can be added here
}

void exitSleepMode()
{
  Serial.println("Exiting sleep mode...");
  // Additional sleep mode exit actions can be added here
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