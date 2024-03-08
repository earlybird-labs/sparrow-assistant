import {BleManager, Characteristic, Device} from 'react-native-ble-plx';

export const serviceUUID = '4fafc201-1fb5-459e-8fcc-c5c9c331914b';
export const characteristicUUID = 'beb5483e-36e1-4688-b7f5-ea07361b26a8';

export const connectToDevice = async (
  manager: BleManager,
  device: Device,
): Promise<void> => {
  try {
    manager.stopDeviceScan();
    const connectedDevice = await manager.connectToDevice(device.id);
    console.log(`Connected to ${connectedDevice.name}`);

    const discoveredDevice =
      await connectedDevice.discoverAllServicesAndCharacteristics();
    console.log(
      `Services and characteristics discovered for ${discoveredDevice.name}`,
    );

    await discoveredDevice.monitorCharacteristicForService(
      serviceUUID,
      characteristicUUID,
      onCharacteristicValueChange,
    );
  } catch (error) {
    console.warn(`Connection error: ${error}`);
    throw error;
  }
};

const onCharacteristicValueChange = (
  error: Error | null,
  characteristic: Characteristic | null,
) => {
  // Handle error scenario
  if (error) {
    console.warn(`Error setting up notifications: ${error}`);
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
    const decodedValue = Buffer.from(characteristic.value, 'base64').toString(
      'ascii',
    );
    console.log(`Decoded data: ${decodedValue}`);
  } else {
    console.log('Characteristic value is null, cannot decode.');
  }
};
