import {useCallback, useEffect, useState} from 'react';
import {BleManager, Device, State} from 'react-native-ble-plx';
import {connectToDevice} from '../services/bleService';
import {requestPermissions} from '../utils/permissions';

type DeviceArray = Device[];

export const useBluetooth = () => {
  const [devices, setDevices] = useState<DeviceArray>([]);
  const [manager, setManager] = useState(new BleManager());
  const [connectedDeviceId, setConnectedDeviceId] = useState<string | null>(
    null,
  );

  const connectToDeviceById = useCallback(
    (deviceId: string) => {
      const device = devices.find(d => d.id === deviceId);
      if (device) {
        connectToDevice(manager, device);
      } else {
        console.warn(`Device with ID ${deviceId} not found.`);
      }
    },
    [devices],
  );

  const reconnectToDevice = useCallback(() => {
    if (connectedDeviceId) {
      connectToDeviceById(connectedDeviceId);
    } else {
      console.warn('No device ID stored for reconnection.');
    }
  }, [connectedDeviceId, connectToDeviceById]);

  useEffect(() => {
    async function initBLE() {
      await requestPermissions();
      const subscription = manager.onStateChange(async (state: State) => {
        if (state === 'PoweredOn') {
          scanAndConnect();
        } else if (state === 'Unknown') {
          console.warn('BLE is in an unknown state, attempting to reset...');
          await new Promise(resolve => setTimeout(resolve, 1000));
          setManager(new BleManager());
        } else if (state === 'PoweredOff') {
          console.warn('Bluetooth is off. Please turn it on to proceed.');
        }
      }, true);

      return () => {
        subscription.remove();
        manager.stopDeviceScan();
      };
    }

    initBLE();
  }, [manager]);

  useEffect(() => {
    if (!connectedDeviceId) return; // Ensure connectedDeviceId is not null

    const disconnectionSubscription = manager.onDeviceDisconnected(
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
  }, [connectedDeviceId, manager]);

  function scanAndConnect() {
    manager.startDeviceScan(null, null, (error, device) => {
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

  async function connectToDeviceWrapper(device: Device) {
    try {
      await connectToDevice(manager, device);
      setConnectedDeviceId(device.id);
    } catch (error) {
      console.warn(`Connection error: ${error}`);
      setConnectedDeviceId(null);
    }
  }

  return {
    devices,
    connectedDeviceId,
    connectToDevice: connectToDeviceWrapper,
  };
};
