import React from 'react';
import {Button, StyleSheet, View} from 'react-native';
import DeviceList from '../components/DeviceList';
import {useBluetooth} from '../hooks/useBluetooth';

function HomeScreen() {
  const {
    devices,
    connectedDeviceId,
    connectToDevice,
    checkConnection,
    disconnectFromDevice,
  } = useBluetooth();

  const handleCheckConnection = () => {
    checkConnection();
  };

  const handleDisconnect = () => {
    disconnectFromDevice();
  };

  return (
    <View style={styles.container}>
      <DeviceList
        devices={devices}
        connectedDeviceId={connectedDeviceId}
        onDevicePress={device => connectToDevice(device.id)}
      />
      {/* Check Connection Button */}
      <Button title="Disconnect" onPress={handleDisconnect} color="#FFFFFF" />
      <Button
        title="Check Connection"
        onPress={handleCheckConnection}
        color="#FFFFFF"
      />
    </View>
  );
}

// Add styles for the button if needed and update the container style to accommodate the button
const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#3C80FF',
    width: '100%',
    alignItems: 'center',
    justifyContent: 'space-between', // Updated to space-between to accommodate the button at the bottom
    paddingTop: '5%',
    paddingBottom: '5%',
  },
  checkConnectionButton: {
    width: '100%',
    backgroundColor: '#002D5E',
  },
});

export default HomeScreen;
