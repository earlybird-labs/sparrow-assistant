import React from 'react';
import {Platform, Text, View} from 'react-native';

export function NavBar() {
  if (Platform.OS === 'web') {
    return null;
  }
  return (
    <View
      style={{
        alignItems: 'center',
        marginTop: 10,
      }}>
      <Text>💬 Gifted Chat{'\n'}</Text>
    </View>
  );
}
