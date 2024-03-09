import {
  BleError,
  BleManager,
  Characteristic,
  Device,
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
      if (this.connectedDevice) {
        const cancelConnection = await this.connectedDevice.cancelConnection();
        console.log(cancelConnection);
      } else {
        this.logMessage(`No connected device found.`);
      }
      this.connectedDevice = null;
    } catch (error) {
      console.warn(`Disconnection error`, error);
      // Check if the error is an instance of BleError and handle accordingly
      if (error instanceof BleError) {
        this.logMessage(
          `Device already disconnected or disconnection operation was cancelled.`,
        );
        this.connectedDevice = null; // Ensure connectedDevice is cleared
      } else {
        // For other errors, you might want to throw or handle differently
        throw error;
      }
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

      await this.discoverAndMonitorDevice(connectedDevice);
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

  async discoverAndMonitorDevice(device: Device): Promise<void> {
    this.logMessage(
      'Attempting to discover all services and characteristics...',
    );
    const discoveredDevice =
      await device.discoverAllServicesAndCharacteristics();
    this.logMessage(
      `Services and characteristics successfully discovered for ${discoveredDevice.name}.`,
    );

    this.logMessage(
      `Attempting to monitor characteristic for service: ${serviceUUID} and characteristic: ${characteristicUUID}...`,
    );
    // try {
    //   const characteristic = await this.readCharacteristic(
    //     discoveredDevice.id,
    //     serviceUUID,
    //     characteristicUUID,
    //   );
    //   console.log(characteristic);
    // } catch (error) {
    //   this.handleError('Failed to read characteristic value', error);
    // }
  }

  onCharacteristicValueChange = (
    error: Error | null,
    characteristic: Characteristic | null,
  ) => {
    if (error) {
      this.handleError(`Error setting up notifications`, error);
      return;
    }

    if (!characteristic) {
      this.logMessage('No characteristic data received.');
      return;
    }

    this.logMessage(`Received data: ${characteristic.value}`);
    if (characteristic.value) {
      const decodedValue = Buffer.from(characteristic.value, 'base64').toString(
        'ascii',
      );
      this.logMessage(`Decoded data: ${decodedValue}`);
    } else {
      this.logMessage('Characteristic value is null, cannot decode.');
    }
  };
}

export default BLEService;
