#include <Arduino.h>
#include "WiFiHandler.h"
#include "WebSocketHandler.h"
#include "AudioHandler.h"
#include "SleepHandler.h"
#include "TouchHandler.h"

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
void touchCallback(bool isSleep);

WiFiHandler wifiHandler(ssid, password);
WebSocketHandler webSocketHandler(websocket_server_host, websocket_server_port);
AudioHandler audioHandler(&webSocketHandler);
SleepHandler sleepHandler;
TouchHandler touchHandler(TOUCH_PIN, touchCallback);

// ADD BACK LATER
// TouchHandler touchHandler(2, touchCallback);

// Define a callback function for touch events
void touchCallback(bool isSleep)
{
  if (isSleep)
  {
    Serial.println("Touch detected: Entering sleep mode");
    enterSleepMode();
  }
  else
  {
    Serial.println("Touch detected: Exiting sleep mode");
    exitSleepMode();
  }
}

void setup()
{
  Serial.begin(115200);
  delay(2000);
  printf("Serial initialized\n");

  touchHandler.begin();
  printf("TouchHandler initialized\n");

  // xTaskCreatePinnedToCore(micTask, "micTask", 10000, NULL, 1, NULL, 1);
  // printf("micTask created\n");

  delay(2000);

  wifiHandler.connect();

  if (wifiHandler.isConnected())
  {
    webSocketHandler.connect();
  }

  delay(1000);
}

void loop()
{
  bool touchChange = touchHandler.checkTouch();
  if (touchChange)
  {
    printf("Touch change detected\n");
    if (!sleepHandler.isSleepMode())
    {
      sleepHandler.enterSleepMode();
    }
    else
    {
      sleepHandler.exitSleepMode();
    }
  }

  delay(3000); // Adjust the delay as needed
}

// void loop()
// {
//   printf("TouchSleepHandler isSleepMode: %d\n", touchSleepHandler.getIsSleepMode());
//   delay(5000);
// }

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