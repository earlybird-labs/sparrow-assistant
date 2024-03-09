#include <Arduino.h>
#include "WiFiHandler.h"
#include "WebSocketHandler.h"
#include "AudioHandler.h"

#define bufferCnt 10
#define bufferLen 1024

extern String receivedSSID;
extern String receivedPassword;
extern bool credentialsReceived;

AudioHandler audioHandler;
int16_t sBuffer[bufferLen];

const char *ssid = "Early Bird Labs";
const char *password = "Hatchling22!";
const char *websocket_server_host = "192.168.4.142";
const uint16_t websocket_server_port = 8888;

WiFiHandler wifiHandler(ssid, password);
WebSocketHandler webSocketHandler(websocket_server_host, websocket_server_port);

void micTask(void *parameter);

void setup()
{
  Serial.begin(115200);
  wifiHandler.connect();
  if (wifiHandler.isConnected())
  {
    webSocketHandler.connect();
  }
  xTaskCreatePinnedToCore(micTask, "micTask", 10000, NULL, 1, NULL, 1);
}

void loop()
{
}

void micTask(void *parameter)
{
  audioHandler.begin(); // Initialize the AudioHandler

  size_t bytesIn = 0;
  while (1)
  {
    audioHandler.readMic(sBuffer, sizeof(sBuffer), bytesIn); // Use AudioHandler to read the mic
    if (bytesIn > 0 && webSocketHandler.isConnected())
    {
      // Check if the AudioHandler is in speaking mode
      if (audioHandler.isSpeaking)
      {
        // If in speaking mode, send the buffer over WebSocket
        webSocketHandler.sendBinary((const char *)sBuffer, bytesIn);
      }
    }
  }
}