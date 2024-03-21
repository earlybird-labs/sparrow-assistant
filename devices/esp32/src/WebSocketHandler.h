#ifndef WebSocketHandler_h
#define WebSocketHandler_h

#include <ArduinoWebsockets.h>
using namespace websockets;

class WebSocketHandler
{
public:
    WebSocketHandler(const char *host, uint16_t port);
    void connect();
    void disconnect();
    bool isConnected();
    void sendBinary(const char *message, size_t length);
    void sendText(const char *message);

private:
    WebsocketsClient client;
    const char *host;
    uint16_t port;
    bool isWebSocketConnected = false;
    static void onEventsCallback(WebsocketsEvent event, String data, void *context);
};

#endif