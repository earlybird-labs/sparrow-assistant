#include "WiFiHandler.h"

/**
 * Constructor for the WiFiHandler class.
 * Initializes the WiFi connection with the provided SSID and password.
 *
 * @param ssid The SSID of the WiFi network to connect to.
 * @param password The password of the WiFi network.
 */
WiFiHandler::WiFiHandler(const char *ssid, const char *password) : ssid(ssid), password(password) {}

/**
 * Attempts to connect to the WiFi network.
 * It tries to connect to the WiFi network specified during the object's construction.
 * If the connection is successful, it prints the device's IP address.
 * If the connection fails after 20 attempts, it prints an error message.
 */
void WiFiHandler::connect()
{
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password); // Start WiFi connection with given credentials.

    int attempts = 0; // Counter for connection attempts.
    // Keep trying to connect until successful or until 20 attempts are made.
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
        delay(500);        // Wait for 500ms before trying again.
        Serial.print("."); // Print a dot as a visual indicator of an attempt.
        attempts++;        // Increment the attempt counter.
    }

    // Check if connected successfully.
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nWiFi connected"); // Inform the user of a successful connection.
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP()); // Print the IP address assigned to the device.
    }
    else
    {
        // If not connected after 20 attempts, inform the user.
        Serial.println("\nFailed to connect to WiFi. Please check your SSID and password.");
    }
}

/**
 * Checks if the device is currently connected to a WiFi network.
 *
 * @return True if the device is connected to a WiFi network, false otherwise.
 */
bool WiFiHandler::isConnected()
{
    return WiFi.status() == WL_CONNECTED; // Return the connection status.
}

/**
 * Disconnects the device from the current WiFi network.
 * It will disconnect the device if it is currently connected to a WiFi network.
 * After disconnecting, it prints a message indicating the device is no longer connected.
 */
void WiFiHandler::disconnect()
{
    if (WiFi.status() == WL_CONNECTED) // Check if the device is currently connected.
    {
        WiFi.disconnect();                    // Disconnect from the WiFi network.
        Serial.println("WiFi disconnected."); // Inform the user of the disconnection.
    }
    else
    {
        Serial.println("Device is not connected to WiFi."); // Inform the user if the device was not connected.
    }
}
