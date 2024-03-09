#include "WebSocketHandler.h"

// Global or static pointer to hold the instance for the callback
static WebSocketHandler *globalWebSocketHandlerInstance = nullptr;

WebSocketHandler::WebSocketHandler(const char *host, uint16_t port) : host(host), port(port)
{
    // Assign the global/static pointer to this instance
    globalWebSocketHandlerInstance = this;
}

void WebSocketHandler::connect()
{
    if (globalWebSocketHandlerInstance)
    {
        client.onEvent([](WebsocketsEvent event, String data)
                       {
            // Pass a nullptr for the context parameter since it's not used
            if (globalWebSocketHandlerInstance) {
                globalWebSocketHandlerInstance->onEventsCallback(event, data, nullptr);
            } });
        while (!client.connect(host, port, "/"))
        {
            delay(500);
            Serial.print(".");
        }
        Serial.println("Websocket Connected!");
    }
}

bool WebSocketHandler::isConnected()
{
    return isWebSocketConnected;
}

void WebSocketHandler::sendText(const char *message)
{
    if (isWebSocketConnected)
    {
        client.send(message, strlen(message));
    }
}

void WebSocketHandler::sendBinary(const char *message, size_t length)
{
    if (isWebSocketConnected)
    {
        client.sendBinary(message, length);
    }
}

// Static member function implementation
void WebSocketHandler::onEventsCallback(WebsocketsEvent event, String data, void *context)
{
    // Use the global/static instance pointer to access instance members
    if (globalWebSocketHandlerInstance)
    {
        switch (event)
        {
        case WebsocketsEvent::ConnectionOpened:
            Serial.println("Connection Opened");
            globalWebSocketHandlerInstance->isWebSocketConnected = true;
            break;
        case WebsocketsEvent::ConnectionClosed:
            Serial.println("Connection Closed");
            globalWebSocketHandlerInstance->isWebSocketConnected = false;
            break;
        default:
            break;
        }
    }
}