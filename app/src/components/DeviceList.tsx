import React from 'react';
import {FlatList, StyleSheet, Text, TouchableOpacity} from 'react-native';
import {Device} from 'react-native-ble-plx';

type DeviceListProps = {
  devices: Device[];
  onDevicePress: (device: Device) => void;
  connectedDeviceId: string | null; // Add this line
};

const DeviceList = ({
  devices,
  onDevicePress,
  connectedDeviceId,
}: DeviceListProps) => {
  return (
    <FlatList
      style={styles.listContentContainer}
      data={devices}
      keyExtractor={item => item.id}
      renderItem={({item}) => (
        <TouchableOpacity
          style={[
            styles.deviceContainer,
            item.id === connectedDeviceId ? styles.connectedDevice : {}, // Apply connectedDevice style if the item is the connected device
          ]}
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
  connectedDevice: {
    // Add this style
    borderColor: 'white',
    borderWidth: 2,
  },
});

export default DeviceList;
