import {
  BleManager,
  Characteristic,
  Device,
  Subscription,
} from 'react-native-ble-plx';

export const serviceUUID = '4fafc201-1fb5-459e-8fcc-c5c9c331914b';
export const characteristicUUID = 'beb5483e-36e1-4688-b7f5-ea07361b26a8';

class BLEService {
  manager: BleManager;
  connectedDevice: Device | null = null;

  constructor() {
    this.manager = new BleManager();
  }

  // Utility method to log and throw errors
  private handleError(message: string, error: any): void {
    console.warn(message, error);
    throw new Error(`${message}: ${error}`);
  }

  // Utility method to log messages
  private logMessage(message: string): void {
    console.log(message);
  }

  async disconnectFromDevice(deviceId: string): Promise<void> {
    this.logMessage(
      `Attempting to disconnect from device with ID: ${deviceId}...`,
    );
    try {
      if (!this.connectedDevice || this.connectedDevice.id !== deviceId) {
        this.logMessage(`No connected device with ID: ${deviceId} found.`);
        return;
      }
      await this.connectedDevice.cancelConnection();
      this.logMessage(
        `Successfully disconnected from device with ID: ${deviceId}.`,
      );
      this.connectedDevice = null; // Ensure the connected device is cleared
    } catch (error) {
      this.handleError(`Disconnection error`, error);
    }
  }

  async connectToDevice(device: Device): Promise<void> {
    this.logMessage('Attempting to stop device scan...');
    this.manager.stopDeviceScan();

    if (this.connectedDevice) {
      await this.disconnectFromDevice(this.connectedDevice.id);
    }

    this.logMessage(`Attempting to connect to device with ID: ${device.id}...`);
    try {
      const connectedDevice = await this.manager.connectToDevice(device.id);
      this.connectedDevice = connectedDevice;
      this.logMessage(`Successfully connected to ${connectedDevice.name}.`);

      await device.discoverAllServicesAndCharacteristics();

      const deviceCharacteristics = await this.manager.characteristicsForDevice(
        connectedDevice.id,
        serviceUUID,
      );

      console.log('deviceCharacteristics', deviceCharacteristics);

      await this.discoverAndMonitorDevice(connectedDevice);

      await this.readCharacteristic(
        connectedDevice.id,
        serviceUUID,
        characteristicUUID,
      );
    } catch (error) {
      this.handleError(`Connection error`, error);
    }
  }

  async readCharacteristic(
    deviceId: string,
    serviceUUID: string,
    characteristicUUID: string,
  ): Promise<Characteristic> {
    this.logMessage(
      `Reading characteristic ${characteristicUUID} for device ${deviceId}...`,
    );
    try {
      const characteristic = await this.manager.readCharacteristicForDevice(
        deviceId,
        serviceUUID,
        characteristicUUID,
      );
      this.logMessage(`Read characteristic value: ${characteristic.value}`);
      return characteristic;
    } catch (error) {
      this.handleError('Failed to read characteristic value', error);
      throw error; // Re-throw the error if you want to handle it further up the call stack.
    }
  }

  async inspectConnection(deviceId: string): Promise<void> {
    this.logMessage(`Inspecting connection for device with ID: ${deviceId}...`);
    try {
      const device = await this.manager.connectedDevices([serviceUUID]);
      const isConnected = device.some(d => d.id === deviceId);
      if (isConnected) {
        this.logMessage(`Device with ID: ${deviceId} is still connected.`);
        // Attempt to re-discover services and characteristics
        await this.discoverAndMonitorDevice(
          device.find(d => d.id === deviceId)!,
        );
      } else {
        this.logMessage(`Device with ID: ${deviceId} is not connected.`);
        // Here, you might want to attempt a reconnection or handle the disconnected state appropriately
      }
    } catch (error) {
      this.handleError('Failed to inspect connection', error);
    }
  }

  async discoverAndMonitorDevice(device: Device): Promise<void> {
    this.logMessage(
      'Attempting to discover all services and characteristics...',
    );
    try {
      const discoveredDevice =
        await device.discoverAllServicesAndCharacteristics();
      const services = await discoveredDevice.services();
      for (const service of services) {
        this.logMessage(`Service UUID: ${service.uuid}`);
        try {
          const characteristics = await service.characteristics();
          this.logMessage(
            `Found ${characteristics.length} characteristics for service ${service.uuid}`,
          );
          for (const characteristic of characteristics) {
            this.logMessage(`-- Characteristic UUID: ${characteristic.uuid}`);
          }
        } catch (error) {
          this.handleError(
            `Failed to list characteristics for service ${service.uuid}`,
            error,
          );
        }
      }
    } catch (error) {
      this.handleError(
        'Failed to discover services and characteristics',
        error,
      );
    }
  }

  async setupNotifications(deviceId: string): Promise<void> {
    this.logMessage(
      `Setting up notifications for device with ID: ${deviceId}...`,
    );
    try {
      const device =
        await this.manager.discoverAllServicesAndCharacteristicsForDevice(
          deviceId,
        );
      const services = await device.services();
      const targetService = services.find(s => s.uuid === serviceUUID);
      if (!targetService) {
        this.logMessage(`Service with UUID: ${serviceUUID} not found.`);
        return;
      }
      const characteristics = await targetService.characteristics();
      const targetCharacteristic = characteristics.find(
        c => c.uuid === characteristicUUID,
      );
      if (!targetCharacteristic) {
        this.logMessage(
          `Characteristic with UUID: ${characteristicUUID} not found.`,
        );
        return;
      }
      if (
        !targetCharacteristic.isNotifiable &&
        !targetCharacteristic.isIndicatable
      ) {
        this.logMessage(
          `Characteristic does not support notifications or indications.`,
        );
        return;
      }

      // Remove any existing subscription if necessary
      if (this.monitorSubscription) {
        this.monitorSubscription.remove();
      }

      // Remove any existing subscription if necessary
      if (this.monitorSubscription) {
        this.monitorSubscription.remove();
      }

      // Set up the monitoring subscription
      this.monitorSubscription =
        await this.manager.monitorCharacteristicForDevice(
          deviceId,
          serviceUUID,
          characteristicUUID,
          (error, characteristic) => {
            if (error) {
              this.logMessage(
                `Error during monitoring: ${JSON.stringify(error)}`,
              );
              return;
            }
            if (characteristic) {
              const value = characteristic.value;
              this.logMessage(`Received characteristic update: ${value}`);
              // If the value is base64 encoded, you can decode it
              if (value) {
                const decodedValue = Buffer.from(value, 'base64').toString(
                  'ascii',
                );
                this.logMessage(`Decoded value: ${decodedValue}`);
              }
            }
          },
        );
      this.logMessage(`Notifications have been set up.`);
    } catch (error) {
      this.handleError('Failed to setup notifications', error);
    }
  }

  // Add a property to hold the subscription
  private monitorSubscription: Subscription | null = null;
}

export default BLEService;
