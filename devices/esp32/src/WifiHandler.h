#ifndef WiFiHandler_h
#define WiFiHandler_h

#include <WiFi.h>

class WiFiHandler
{
public:
    WiFiHandler(const char *ssid, const char *password);
    void connect();
    bool isConnected();

private:
    const char *ssid;
    const char *password;
};

#endif