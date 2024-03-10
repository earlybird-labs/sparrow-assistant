#include <Arduino.h>
#include "WiFiHandler.h"
#include "WebSocketHandler.h"
#include "AudioHandler.h"
#include "TouchSleepHandler.h"

#define bufferLen 1024

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

WiFiHandler wifiHandler(ssid, password);
WebSocketHandler webSocketHandler(websocket_server_host, websocket_server_port);
AudioHandler audioHandler(&webSocketHandler);
TouchSleepHandler touchSleepHandler(enterSleepMode, exitSleepMode);

void setup()
{
  Serial.begin(115200);          // Start the serial monitor
  wifiHandler.connect();         // Connect to the WiFi network
  if (wifiHandler.isConnected()) // Check if the WiFi network is connected
  {
    webSocketHandler.connect(); // Connect to the WebSocket server
  }
  xTaskCreatePinnedToCore(micTask, "micTask", 10000, NULL, 1, NULL, 1); // Create a new task for the mic
}

void loop()
{
  // touchSleepHandler.handleTouch(); // Check touch sensor and handle sleep mode
  // delay(3000);                     // Delay to reduce constant polling, adjust as needed
}

void enterSleepMode()
{
  Serial.println("Entering sleep mode...");
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

  size_t bytesIn = 0;       // Number of bytes read from the mic
  bool wasSpeaking = false; // Track speaking state change
  while (1)                 // Loop forever
  {
    audioHandler.readMic(sBuffer, sizeof(sBuffer), bytesIn); // Use AudioHandler to read the mic
    if (bytesIn > 0 && webSocketHandler.isConnected())       // Check if the number of bytes read from the mic is greater than 0 and if the WebSocket is connected
    {
      if (audioHandler.getIsSpeaking()) // If the AudioHandler is in speaking mode
      {
        if (!wasSpeaking) // Just transitioned to speaking mode
        {
          // Retrieve and send pre-buffered data first
          int16_t *preBufferData;
          size_t preBufferLength;
          audioHandler.getPreBufferData(&preBufferData, &preBufferLength); // Assume this method is implemented in AudioHandler
          webSocketHandler.sendBinary((const char *)preBufferData, preBufferLength * sizeof(int16_t));

          wasSpeaking = true;
        }
        // Then send current buffer
        webSocketHandler.sendBinary((const char *)sBuffer, bytesIn);
      }
      else
      {
        wasSpeaking = false;
      }
    }
  }
}