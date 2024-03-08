import React, {useEffect, useState} from 'react';
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

async function requestPermissions() {
  if (Platform.OS === 'android' && Platform.Version >= 23) {
    await PermissionsAndroid.request(
      PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
    );
  }
}

function App() {
  // Specify the type for devices state
  const [devices, setDevices] = useState<DeviceArray>([]);
  const [manager, setManager] = useState(new BleManager());
  const [connectedDeviceId, setConnectedDeviceId] = useState<string | null>(
    null,
  );

  useEffect(() => {
    async function initBLE() {
      await requestPermissions();
      const subscription = manager.onStateChange(async (state: State) => {
        if (state === 'PoweredOn') {
          scanAndConnect();
        } else if (state === 'Unknown') {
          console.warn('BLE is in an unknown state, attempting to reset...');
          // Attempt to reset the BLE manager with a delay
          await new Promise(resolve => setTimeout(resolve, 1000)); // Wait for 1 second
          setManager(new BleManager());
          // Optionally, prompt the user to check their Bluetooth settings
        } else if (state === 'PoweredOff') {
          // Inform the user that Bluetooth needs to be enabled
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

  function scanAndConnect() {
    manager.startDeviceScan(null, null, (error, device) => {
      if (error) {
        console.warn(error);
        return;
      }
      if (device && device.name !== null) {
        // Check if device name is not 'Unnamed device'
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

  function connectToDevice(device: Device) {
    // Define the type for device parameter
    manager.stopDeviceScan();
    manager
      .connectToDevice(device.id)
      .then(() => {
        console.log(`Connected to ${device.name}`);
        setConnectedDeviceId(device.id);
        // Handle successful connection
      })
      .catch(error => {
        console.warn(error);
      });
  }

  return (
    <View style={styles.container}>
      <FlatList<Device>
        data={devices}
        keyExtractor={(item: Device) => item.id}
        renderItem={({item}) => (
          <TouchableOpacity
            style={[
              styles.deviceContainer,
              // Apply the connectedDeviceStyle if this device is connected
              item.id === connectedDeviceId && styles.connectedDeviceStyle,
            ]}
            onPress={() => connectToDevice(item)}>
            <Text style={styles.deviceText}>
              {item.name || 'Unnamed device'}
            </Text>
          </TouchableOpacity>
        )}
        contentContainerStyle={styles.listContentContainer} // Add padding to the FlatList content
      />
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#007bff',
    width: '100%', // Ensures the container takes the full width of the screen
    alignItems: 'center',
    justifyContent: 'center',
    paddingTop: '5%', // Added 5% padding at the top
    paddingBottom: '5%', // Added 5% padding at the bottom
  },
  deviceText: {
    color: '#ffffff',
    fontSize: 20,
  },
  // Adjusted deviceContainer style
  deviceContainer: {
    padding: 10,
    marginVertical: 5,
    backgroundColor: '#0056b3',
    borderRadius: 5,
    width: '100%', // Ensures the container takes the full width of the screen
    paddingHorizontal: 32, // Removes horizontal padding to utilize full screen width
  },
  // New style for connected device
  connectedDeviceStyle: {
    borderColor: '#ffffff',
    borderWidth: 2,
  },
  // New style for FlatList content padding
  listContentContainer: {
    paddingVertical: '15%', // Add padding to prevent cutting off
  },
});

export default App;
