import {useCallback, useEffect, useState} from 'react';
import {Device, State} from 'react-native-ble-plx';
import BLEService from '../services/bleService'; // Import the BLEService class
import {requestPermissions} from '../utils/permissions';

type DeviceArray = Device[];

export const useBluetooth = () => {
  const [devices, setDevices] = useState<DeviceArray>([]);
  const [bleService] = useState(new BLEService()); // Initialize BLEService
  const [connectedDeviceId, setConnectedDeviceId] = useState<string | null>(
    null,
  );

  const connectToDeviceById = useCallback(
    (deviceId: string) => {
      const device = devices.find(d => d.id === deviceId);
      if (device) {
        bleService
          .connectToDevice(device)
          .then(() => {
            setConnectedDeviceId(device.id); // Update connected device ID on successful connection
          })
          .catch(console.warn);
      } else {
        console.warn(`Device with ID ${deviceId} not found.`);
      }
    },
    [devices, bleService],
  );

  const disconnectFromDevice = useCallback(() => {
    if (connectedDeviceId) {
      bleService.disconnectFromDevice(connectedDeviceId);
    }
  }, [connectedDeviceId, bleService]);

  const reconnectToDevice = useCallback(async () => {
    if (connectedDeviceId) {
      try {
        await bleService.disconnectFromDevice(connectedDeviceId); // Ensure disconnection
      } catch (error) {
        console.warn(`Failed to disconnect before reconnecting: ${error}`);
        // Even if disconnection fails, you might still want to try reconnecting
      }
      connectToDeviceById(connectedDeviceId);
    } else {
      console.warn('No device ID stored for reconnection.');
    }
  }, [connectedDeviceId, connectToDeviceById, bleService]);

  useEffect(() => {
    async function initBLE() {
      await requestPermissions();
      const subscription = bleService.manager.onStateChange(
        async (state: State) => {
          if (state === 'PoweredOn') {
            scanAndConnect();
          } else if (state === 'Unknown') {
            console.warn('BLE is in an unknown state, attempting to reset...');
            await new Promise(resolve => setTimeout(resolve, 1000));
            // Re-initialize BLEService to reset the manager
            bleService.manager = new BLEService().manager;
          } else if (state === 'PoweredOff') {
            console.warn('Bluetooth is off. Please turn it on to proceed.');
          }
        },
        true,
      );

      return () => {
        subscription.remove();
        bleService.manager.stopDeviceScan();
      };
    }

    initBLE();
  }, [bleService]);

  useEffect(() => {
    if (!connectedDeviceId) return; // Ensure connectedDeviceId is not null

    const disconnectionSubscription = bleService.manager.onDeviceDisconnected(
      connectedDeviceId,
      (error, device) => {
        if (error) {
          console.warn(`Disconnection error: ${error.message}`);
          return;
        }
        console.log(
          `Device ${device?.name} disconnected. Attempting to reconnect...`,
        );
        setConnectedDeviceId(null);
        if (device) {
          reconnectToDevice();
        }
      },
    );

    return () => {
      disconnectionSubscription.remove();
    };
  }, [connectedDeviceId, bleService]);

  function scanAndConnect() {
    bleService.manager.startDeviceScan(null, null, (error, device) => {
      if (error) {
        console.warn(error);
        return;
      }
      if (device && device.name !== null) {
        setDevices((prevDevices: DeviceArray) => {
          const deviceExists = prevDevices.some(
            prevDevice => prevDevice.id === device.id,
          );
          if (!deviceExists) {
            return [...prevDevices, device];
          }
          return prevDevices;
        });
      }
    });
  }
  // Inside useBluetooth hook, add the following function

  const toggleDeviceConnection = useCallback(
    (deviceId: string) => {
      if (connectedDeviceId === deviceId) {
        disconnectFromDevice(); // Disconnect if it's the same device
      } else {
        connectToDeviceById(deviceId); // Connect if it's a different device
      }
    },
    [connectToDeviceById, disconnectFromDevice, connectedDeviceId],
  );

  // Update the return statement of the hook to include toggleDeviceConnection
  return {
    devices,
    connectedDeviceId,
    connectToDevice: toggleDeviceConnection, // Use toggleDeviceConnection here
    disconnectFromDevice,
  };
};
