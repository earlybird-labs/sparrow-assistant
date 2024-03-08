import React from 'react';
import {StyleSheet, View} from 'react-native';
import DeviceList from '../components/DeviceList';
import {useBluetooth} from '../hooks/useBluetooth';

function HomeScreen() {
  const {devices, connectedDeviceId, connectToDevice} = useBluetooth();

  return (
    <View style={styles.container}>
      <DeviceList devices={devices} onDevicePress={connectToDevice} />
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
});

export default HomeScreen;