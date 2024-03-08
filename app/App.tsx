import React, {useCallback, useEffect, useState} from 'react';
import {
  FlatList,
  PermissionsAndroid,
  Platform,
  StyleSheet,
  Text,
  TouchableOpacity,
  View,
} from 'react-native';
import {BleManager, Device, State} from 'react-native-ble-plx';

// Define a type for our device state to resolve the type error
type DeviceArray = Device[];

// Request permissions for Android devices
async function requestPermissions() {
  if (Platform.OS === 'android' && Platform.Version >= 23) {
    await PermissionsAndroid.request(
      PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
    );
  }
}

function App() {
  const [devices, setDevices] = useState<DeviceArray>([]);
  const [manager, setManager] = useState(new BleManager());
  const [connectedDeviceId, setConnectedDeviceId] = useState<string | null>(
    null,
  );

  // Initialize BLE on component mount
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
        // Attempt to reconnect or update UI accordingly
        setConnectedDeviceId(null); // Reset connected device ID
        // Optionally, trigger a reconnection attempt here
      },
    );

    return () => {
      disconnectionSubscription.remove();
    };
  }, [connectedDeviceId, manager]);

  // Scan for devices and update state
  function scanAndConnect() {
    manager.startDeviceScan(null, null, (error, device) => {
      if (error) {
        console.warn(error);
        return;
      }
      if (device && device.name !== null) {
        setDevices((prevDevices: DeviceArray) => {
          const deviceExists = prevDevices.some(
            (prevDevice: Device) => prevDevice.id === device.id,
          );
          if (!deviceExists) {
            return [...prevDevices, device];
          }
          return prevDevices;
        });
      }
    });
  }

  // Connect to a device and listen for data
  const connectToDevice = useCallback(
    (device: Device) => {
      if (connectedDeviceId === device.id) {
        // If the device is already connected, attempt to disconnect
        manager
          .cancelDeviceConnection(device.id)
          .then(() => {
            console.log(`Disconnected from ${device.name}`);
            setConnectedDeviceId(null); // Reset the connected device ID
          })
          .catch(error => {
            console.warn(error);
            setConnectedDeviceId(null); // Reset even if disconnection fails
          });
      } else {
        // Connect to the device if not already connected
        manager.stopDeviceScan();
        manager
          .connectToDevice(device.id)
          .then(() => {
            console.log(`Connected to ${device.name}`);
            setConnectedDeviceId(device.id);
            // After successful connection, setup notifications
            setupNotifications(device.id);
          })
          .catch(error => {
            console.warn(error);
          });
      }
    },
    [connectedDeviceId, manager],
  );

  // Setup notifications to listen for data from the device
  const setupNotifications = useCallback(
    (deviceId: string) => {
      const serviceUUID = '4fafc201-1fb5-459e-8fcc-c5c9c331914b'; // Replace with your service UUID
      const characteristicUUID = 'beb5483e-36e1-4688-b7f5-ea07361b26a8'; // Replace with your characteristic UUID
      manager.monitorCharacteristicForDevice(
        deviceId,
        serviceUUID,
        characteristicUUID,
        (error, characteristic) => {
          if (error) {
            console.warn(error.message);
            return;
          }
          if (characteristic?.value) {
            const data = Buffer.from(characteristic.value, 'base64').toString(
              'ascii',
            );
            console.log(`Received data: ${data}`);
          }
        },
      );
    },
    [manager],
  );

  return (
    <View style={styles.container}>
      <FlatList<Device>
        data={devices}
        keyExtractor={(item: Device) => item.id}
        renderItem={({item}) => (
          <TouchableOpacity
            style={[
              styles.deviceContainer,
              item.id === connectedDeviceId && styles.connectedDeviceStyle,
            ]}
            onPress={() => connectToDevice(item)}>
            <Text style={styles.deviceText}>
              {item.name || 'Unnamed device'}
            </Text>
          </TouchableOpacity>
        )}
        contentContainerStyle={styles.listContentContainer}
      />
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#007bff',
    width: '100%',
    alignItems: 'center',
    justifyContent: 'center',
    paddingTop: '5%',
    paddingBottom: '5%',
  },
  deviceText: {
    color: '#ffffff',
    fontSize: 20,
  },
  deviceContainer: {
    padding: 10,
    marginVertical: 5,
    backgroundColor: '#0056b3',
    borderRadius: 5,
    width: '100%',
    paddingHorizontal: 32,
  },
  connectedDeviceStyle: {
    borderColor: '#ffffff',
    borderWidth: 2,
  },
  listContentContainer: {
    paddingVertical: '15%',
  },
});

export default App;
