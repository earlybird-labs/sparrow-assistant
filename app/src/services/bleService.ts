import {BleManager, Characteristic, Device} from 'react-native-ble-plx';

export const serviceUUID = '4fafc201-1fb5-459e-8fcc-c5c9c331914b';
export const characteristicUUID = 'beb5483e-36e1-4688-b7f5-ea07361b26a8';

class BLEService {
  manager: BleManager;
  connectedDevice: Device | null = null; // Changed from connectedDeviceId to connectedDevice
  connectedDeviceId: string | null = null;

  constructor() {
    this.manager = new BleManager();
  }

  async disconnectFromDevice(deviceId: string): Promise<void> {
    console.log(`Attempting to disconnect from device with ID: ${deviceId}...`);
    try {
      const device = await this.manager.cancelDeviceConnection(deviceId);
      console.log(`Successfully disconnected from device: ${device.name}.`);
      this.connectedDeviceId = null; // Reset connected device ID on disconnect
    } catch (error) {
      console.warn(`Disconnection error: ${error}`);
      this.connectedDeviceId = null; // Ensure the connected device ID is reset

      // Retry disconnection after a short delay
      setTimeout(async () => {
        try {
          await this.manager.cancelDeviceConnection(deviceId);
          console.log('Successfully disconnected after retry.');
        } catch (retryError) {
          console.warn(`Retry disconnection error: ${retryError}`);
          // Consider resetting BleManager or other recovery strategies here
        }
      }, 1000); // Adjust delay as needed
    }
  }

  async connectToDevice(device: Device): Promise<void> {
    try {
      console.log('Attempting to stop device scan...');
      this.manager.stopDeviceScan();
      console.log('Device scan stopped.');

      try {
        device.cancelConnection();
      } catch (error) {
        console.warn(`Error cancelling existing connection: ${error}`);
      }

      // Cancel any existing connections before starting a new connection attempt
      if (this.connectedDeviceId) {
        await this.manager.cancelDeviceConnection(this.connectedDeviceId);
        console.log(
          `Cancelled existing connection with ID: ${this.connectedDeviceId}`,
        );
      }

      console.log(`Attempting to connect to device with ID: ${device.id}...`);
      const connectedDevice = await this.manager.connectToDevice(device.id);
      console.log(`Successfully connected to ${connectedDevice.name}.`);
      this.connectedDeviceId = connectedDevice.id; // Store connected device ID
      this.connectedDevice = connectedDevice;

      console.log('Attempting to discover all services and characteristics...');
      const discoveredDevice =
        await connectedDevice.discoverAllServicesAndCharacteristics();
      console.log(
        `Services and characteristics successfully discovered for ${discoveredDevice.name}.`,
      );

      console.log(
        `Attempting to monitor characteristic for service: ${serviceUUID} and characteristic: ${characteristicUUID}...`,
      );
      const subscription =
        await discoveredDevice.monitorCharacteristicForService(
          serviceUUID,
          characteristicUUID,
          this.onCharacteristicValueChange.bind(this),
        );
      console.log('Monitoring setup successfully.');
      if (subscription) {
        subscription.remove();
      } else {
        console.log('No subscription available for removal.');
      }
    } catch (error) {
      console.warn(`Connection error: ${error}`);
      throw error;
    }
  }

  onCharacteristicValueChange = (
    error: Error | null,
    characteristic: Characteristic | null,
  ) => {
    console.log(
      `Characteristic value changed. Characteristic: ${characteristic}. Error: ${error}`,
    );
    try {
      // Handle error scenario
      if (error) {
        console.warn(`Error setting up notifications: ${error}`);
        if (this.connectedDevice) {
          this.connectedDevice
            .cancelConnection()
            .then(() => {
              console.log(
                'Disconnected due to error in characteristic value change.',
              );
              this.connectedDevice = null; // Reset the connected device reference
            })
            .catch(disconnectError => {
              console.warn(`Error disconnecting device: ${disconnectError}`);
            });
        }
        return;
      }

      // Proceed only if characteristic is not null
      if (!characteristic) {
        console.log('No characteristic data received.');
        return;
      }

      // Log received data
      console.log(`Received data: ${characteristic.value}`);

      // Decode characteristic value if not null
      if (characteristic.value) {
        console.log('Attempting to decode characteristic value...');
        const decodedValue = Buffer.from(
          characteristic.value,
          'base64',
        ).toString('ascii');
        console.log(`Decoded data: ${decodedValue}`);
      } else {
        console.log('Characteristic value is null, cannot decode.');
      }
    } catch (error) {
      console.warn(`Error processing characteristic value change: ${error}`);
    }
  };
}

export default BLEService;
