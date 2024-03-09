#include "WiFiHandler.h"

WiFiHandler::WiFiHandler(const char *ssid, const char *password) : ssid(ssid), password(password) {}

void WiFiHandler::connect()
{
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nWiFi connected");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("\nFailed to connect to WiFi. Please check your SSID and password.");
    }
}

bool WiFiHandler::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}