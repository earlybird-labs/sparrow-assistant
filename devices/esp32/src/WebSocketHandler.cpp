#include "WebSocketHandler.h"

// Global or static pointer to hold the instance for the callback
static WebSocketHandler *globalWebSocketHandlerInstance = nullptr;

// Constructor: Initializes the WebSocketHandler with a host and port
WebSocketHandler::WebSocketHandler(const char *host, uint16_t port) : host(host), port(port)
{
    // Assign the global/static pointer to this instance
    globalWebSocketHandlerInstance = this;
}

// Connects to the WebSocket server
void WebSocketHandler::connect()
{
    // Check if the global instance is available
    if (globalWebSocketHandlerInstance)
    {
        // Set up the event handler for WebSocket events. This lambda function will be called on various WebSocket events.
        client.onEvent([](WebsocketsEvent event, String data)
                       {
            // Inside the lambda, check if the global instance is still valid. This is necessary to ensure that the instance exists when the event occurs.
            if (globalWebSocketHandlerInstance) {
                // If the instance is valid, delegate the handling of the event to the `onEventsCallback` method of the instance.
                globalWebSocketHandlerInstance->onEventsCallback(event, data, nullptr);
            } });
        // Attempt to connect to the WebSocket server. If the connection is not immediately successful, retry every 500 milliseconds.
        while (!client.connect(host, port, "/"))
        {
            delay(500);        // Wait for 500 milliseconds before retrying
            Serial.print("."); // Print a dot to the serial monitor to indicate a retry attempt
        }
        Serial.println("Websocket Connected!"); // Once connected, print a confirmation message to the serial monitor.
    }
}

/**
 * Checks if the WebSocket connection is currently established.
 *
 * @return True if the WebSocket is connected, false otherwise.
 */
bool WebSocketHandler::isConnected()
{
    return isWebSocketConnected;
}

/**
 * Sends a text message over the WebSocket connection if it is established.
 *
 * @param message The message to be sent as a C-style string.
 */
void WebSocketHandler::sendText(const char *message)
{
    // Only attempt to send the message if the WebSocket is connected.
    if (isWebSocketConnected)
    {
        // Send the message as a text frame.
        client.send(message, strlen(message));
    }
}

/**
 * Sends a binary message over the WebSocket connection if it is established.
 *
 * @param message The binary message to be sent.
 * @param length The length of the binary message in bytes.
 */
void WebSocketHandler::sendBinary(const char *message, size_t length)
{
    // Example of splitting into chunks (simplified, adjust based on your needs)
    const size_t maxChunkSize = 1024; // Adjust based on your WebSocket server's capabilities
    for (size_t i = 0; i < length; i += maxChunkSize)
    {
        size_t chunkSize = ((i + maxChunkSize) < length) ? maxChunkSize : (length - i);
        if (isWebSocketConnected)
        {
            client.sendBinary(message + i, chunkSize);
        }
    }
}

/**
 * Static member function to handle WebSocket events.
 * This function is called upon receiving events from the WebSocket connection.
 *
 * @param event The event type received from the WebSocket.
 * @param data Additional data associated with the event.
 * @param context User-defined context, unused in this implementation.
 */
void WebSocketHandler::onEventsCallback(WebsocketsEvent event, String data, void *context)
{
    // Use the global/static instance pointer to access instance members.
    if (globalWebSocketHandlerInstance)
    {
        // Handle the event based on its type.
        switch (event)
        {
        case WebsocketsEvent::ConnectionOpened:
            // Log the connection opening and set the connection status.
            Serial.println("Connection Opened");
            globalWebSocketHandlerInstance->isWebSocketConnected = true;
            break;
        case WebsocketsEvent::ConnectionClosed:
            // Log the connection closing and update the connection status.
            Serial.println("Connection Closed");
            globalWebSocketHandlerInstance->isWebSocketConnected = false;
            break;
        default:
            // For other events, no action is taken.
            break;
        }
    }
}