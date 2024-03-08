import React, {useEffect, useState} from 'react';
import {
  Button,
  FlatList,
  Platform,
  SafeAreaView,
  StyleSheet,
  Text,
  View,
} from 'react-native';
import {BleManager, Device} from 'react-native-ble-plx';
import {PERMISSIONS, RESULTS, check, request} from 'react-native-permissions';

const DEVICE_NAME = 'ESP32';
const SERVICE_UUID = '4fafc201-1fb5-459e-8fcc-c5c9c331914b';
const CHARACTERISTIC_UUID = 'beb5483e-36e1-4688-b7f5-ea07361b26a8';

function App() {
  const [devices, setDevices] = useState<Device[]>([]);
  const [isScanning, setIsScanning] = useState(false);
  const manager = new BleManager();

  useEffect(() => {
    // Function to request permissions based on platform
    const requestPermissions = async () => {
      let permission;
      if (Platform.OS === 'ios') {
        permission = PERMISSIONS.IOS.BLUETOOTH;
      } else {
        permission = PERMISSIONS.ANDROID.ACCESS_FINE_LOCATION;
      }

      const result = await check(permission);
      if (result === RESULTS.DENIED) {
        const status = await request(permission);
        if (status === RESULTS.GRANTED) {
          console.log('Permission granted');
        } else {
          console.warn('Permission denied');
        }
      } else {
        console.log('Permission already granted');
      }
    };

    requestPermissions();

    // Cleanup function to stop scanning and destroy manager on component unmount
    return () => {
      manager.stopDeviceScan();
      manager.destroy();
    };
  }, []);

  const handleScanDevices = () => {
    console.log('Scanning started...');
    setIsScanning(true);

    manager.startDeviceScan(null, null, (error, scannedDevice) => {
      if (error) {
        console.error('Scan error:', error);
        setIsScanning(false);
        return;
      }

      if (scannedDevice) {
        console.log('Device found:', scannedDevice.name, scannedDevice.id);
        // Optionally, filter devices here if you're looking for specific ones
        // For example, if you only want to add devices with a specific name:
        // if (scannedDevice.name && scannedDevice.name.includes('ESP32')) {
        //   setDevices(prevDevices => [...prevDevices, scannedDevice]);
        // }
      }
    });

    // Log the scanning state after a delay to ensure it's still active
    setTimeout(() => {
      console.log('Scanning state:', isScanning ? 'Active' : 'Stopped');
      if (isScanning) {
        console.log('Scanning stopped (timeout).');
        manager.stopDeviceScan();
        setIsScanning(false);
      }
    }, 10000); // Adjust timeout as needed
  };

  return (
    <SafeAreaView style={styles.container}>
      <Button
        title="Scan for Devices"
        onPress={handleScanDevices}
        disabled={isScanning}
      />
      <FlatList
        data={devices}
        keyExtractor={item => item.id}
        renderItem={({item}) => (
          <View style={styles.device}>
            <Text style={styles.deviceName}>
              {item.name || 'Unnamed device'}
            </Text>
            <Text>{item.id}</Text>
          </View>
        )}
      />
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    marginTop: 32,
  },
  device: {
    padding: 20,
    borderBottomWidth: 1,
    borderBottomColor: '#ccc',
  },
  deviceName: {
    fontSize: 18,
    fontWeight: 'bold',
  },
});

export default App;
