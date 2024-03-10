#include "SleepHandler.h"
#include <Arduino.h>
#include "driver/touch_pad.h"
#include "esp_sleep.h"

#define THRESHOLD 40 // Adjust based on your touch sensor's sensitivity
#define TIME_TO_SLEEP 60 // Time ESP32 will go to sleep (in seconds)
#define T3 // Touch pad pin

RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR bool wasWifiConnected = false;
RTC_DATA_ATTR bool wasWebSocketConnected = false;

SleepHandler::SleepHandler() {}

void SleepHandler::begin(int touchPin, uint64_t timeToSleepInSeconds) {
    // Initialize touch pad for wakeup
    touch_pad_init();
    // touch_pad_config(T3, THRESHOLD);
    touch_pad_config(static_cast<touch_pad_t>(touchPin));
    // Set threshold for the touchPad
    touch_pad_set_thresh(static_cast<touch_pad_t>(touchPin), THRESHOLD);
    esp_sleep_enable_touchpad_wakeup();
    esp_sleep_enable_timer_wakeup(timeToSleepInSeconds * 1000000);
}

void SleepHandler::goToDeepSleep() {
    Serial.println("Going to sleep now");
    Serial.flush(); // Wait for serial output to complete
    esp_deep_sleep_start(); // Go to deep sleep
}


// void setup() {
//   esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
//   switch(wakeup_reason){
//     case ESP_SLEEP_WAKEUP_TOUCHPAD:
//       Serial.println("Wakeup caused by touchpad");
//       break;
//     default:
//       Serial.println("Wakeup was not caused by touchpad");
//       break;

//   lastActivityTime = millis(); // Initialize last activity time for the serial inactivity logic
    
//   }

//   // Initialize touch pad for wakeup
//   touch_pad_init();
//   touch_pad_config(T3, THRESHOLD);
//   esp_sleep_enable_touchpad_wakeup();

//   // Attempt to reconnect to WiFi and WebSocket if they were connected before sleep
//   if(wasWifiConnected) {
//     // Code to reconnect to WiFi
//   }
//   if(wasWebSocketConnected) {
//     // Code to reconnect to WebSocket
//   }

//   // Update last serial activity time
//   lastSerialActivity = esp_timer_get_time();

// }

// void loop () {
// esp_sleep_wakeup_cause_t DeepSleepHandler::getWakeupReason() {
//     return esp_sleep_get_wakeup_cause();
// }

//   unsigned long currentTime = millis();
//   static unsigned long lastActivityTime = currentTime;
  
//   // Check if there's any incoming serial data
//   if (Serial.available() > 0) {
//     lastActivityTime = currentTime; // Update the last activity time
//     while(Serial.available()) Serial.read(); // Clear the serial buffer
//   }

//   if (currentTime - lastActivityTime > TIME_TO_SLEEP * 1000) {
//     Serial.println("Going to sleep now");
//     Serial.flush(); // Wait for serial output to complete
    
//     // Update states before sleeping
//     wasWifiConnected = wifiHandler.isConnected();
//     wasWebSocketConnected = webSocketHandler.isConnected();

//     esp_deep_sleep_start(); // Go to deep sleep
//   }