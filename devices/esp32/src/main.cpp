#include <Arduino.h>
#include "WiFiHandler.h"
#include "WebSocketHandler.h"
#include "AudioHandler.h"

#define bufferLen 1024

extern String receivedSSID;
extern String receivedPassword;
extern bool credentialsReceived;

int16_t sBuffer[bufferLen];

const char *ssid = "Early Bird Labs";
const char *password = "Hatchling22!";
const char *websocket_server_host = "192.168.4.142";
const uint16_t websocket_server_port = 8888;

WiFiHandler wifiHandler(ssid, password);
WebSocketHandler webSocketHandler(websocket_server_host, websocket_server_port);
AudioHandler audioHandler(&webSocketHandler);

void micTask(void *parameter);

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