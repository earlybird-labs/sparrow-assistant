import asyncio
from bleak import BleakClient, BleakScanner

# Replace these with your device's address and the characteristic's UUID
ADDRESS = "3BDAE0D3-A4BD-1A6A-AD13-DAC8BE891C37"
# SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"


async def find_devices():
    """Scan for BLE devices and print them out."""
    devices = await BleakScanner.discover()
    for device in devices:
        print(f"Found device: {device.name}, Address: {device.address}")


async def run(address, uuid):
    async with BleakClient(address) as client:
        # Check if we're connected
        if await client.is_connected():
            print(f"Connected to {address}")

            # Function to handle notifications from the device
            def notification_handler(sender, data):
                print(f"Received data: {data}")

            # Start listening for notifications on the characteristic
            await client.start_notify(uuid, notification_handler)

            # Keep the script running while it listens for notifications
            await asyncio.sleep(30)

            # Stop listening for notifications
            await client.stop_notify(uuid)
        else:
            print("Failed to connect")


# asyncio.run(find_devices())

loop = asyncio.get_event_loop()
loop.run_until_complete(run(ADDRESS, CHARACTERISTIC_UUID))
