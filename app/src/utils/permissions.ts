import { PermissionsAndroid, Platform } from 'react-native';

export const requestPermissions = async (): Promise<void> => {
  if (Platform.OS === 'android' && Platform.Version >= 23) {
    await PermissionsAndroid.request(
      PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
    );
  }
};