using Plugin.BLE;
using Plugin.BLE.Abstractions.Contracts;

namespace ClimaLog_App_MAUI.Services
{

    public class BtSerialService
    {
        private readonly IBluetoothLE bluetoothLE;
        private IList<IDevice> foundDevices;
        private IDevice esp32;

        public BtSerialService()
        {
            //Set bluetooth field:
            bluetoothLE = CrossBluetoothLE.Current;
            foundDevices = new List<IDevice>();
        }
        public async Task<bool> ConnectAsync(string deviceName)
        {
            await ScanAllDevicesAsync();
            var device = foundDevices.FirstOrDefault(d => d.Name == deviceName);
            if (device == null)
            {
                throw new Exception("Inside Measurer not found");

            }
            esp32 = device;
            return true;
        }

        public async Task<string> ReceiveMessageAsync()
        {
            throw new NotImplementedException();
        }

        public async Task SendMessageAsync(string message)
        {
            throw new NotImplementedException();
        }
        private async Task ScanAllDevicesAsync()
        {
            foundDevices.Clear();
            var adapter = CrossBluetoothLE.Current.Adapter;

            adapter.DeviceDiscovered += (s, a) =>
            {
                if (!foundDevices.Contains(a.Device))
                    foundDevices.Add(a.Device);
            };

            await adapter.StartScanningForDevicesAsync();
        }
    }
}
