import {useEffect} from 'react';
import {PermissionsAndroid, Platform} from 'react-native';
import {BleManager} from 'react-native-ble-plx';

async function requestPermissions() {
  if (Platform.OS === 'android' && Platform.Version >= 23) {
    await PermissionsAndroid.request(
      PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
    );
  }
}

function App() {
  useEffect(() => {
    requestPermissions();
    const manager = new BleManager();

    manager.onStateChange(state => {
      if (state === 'PoweredOn') {
        console.log('Bluetooth is powered on. Starting scan...');
        manager.startDeviceScan(null, null, (error, device) => {
          if (error) {
            console.warn(error);
            return;
          }
          console.log('Found device:', device?.name, device?.id);
        });
      }
    }, true);

    return () => manager.stopDeviceScan();
  }, []);

  return null; // No UI components
}

export default App;
