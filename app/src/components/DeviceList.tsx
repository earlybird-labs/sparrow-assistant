import React from 'react';
import {FlatList, StyleSheet, Text, TouchableOpacity} from 'react-native';
import {Device} from 'react-native-ble-plx';

type DeviceListProps = {
  devices: Device[];
  onDevicePress: (device: Device) => void;
};

const DeviceList = ({devices, onDevicePress}: DeviceListProps) => {
  return (
    <FlatList
      style={styles.listContentContainer}
      data={devices}
      keyExtractor={item => item.id}
      renderItem={({item}) => (
        <TouchableOpacity
          style={styles.deviceContainer}
          onPress={() => onDevicePress(item)}>
          <Text style={styles.deviceName}>{item.name}</Text>
        </TouchableOpacity>
      )}
    />
  );
};

const styles = StyleSheet.create({
  deviceName: {
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
  listContentContainer: {
    paddingVertical: '15%',
  },
});

export default DeviceList;
