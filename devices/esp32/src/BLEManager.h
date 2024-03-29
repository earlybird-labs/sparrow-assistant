#ifndef BLEMANAGER_H
#define BLEMANAGER_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

class BLEManager : public BLEServerCallbacks, public BLECharacteristicCallbacks
{
public:
    BLEManager(const char *deviceName, const char *serviceUUID, const char *characteristicUUID);
    void setupBLE();
    void onWrite(BLECharacteristic *pCharacteristic) override;
    void onConnect(BLEServer *pServer) override;
    void onDisconnect(BLEServer *pServer) override;
    void sendData(const uint8_t *data, size_t length);
    void sendStartMessage();
    void sendEndMessage();
    void stopBLE();

private:
    std::string deviceName;
    std::string serviceUUID;
    std::string characteristicUUID;
    BLECharacteristic *pCharacteristic = nullptr;
};

#endif // BLEMANAGER_H