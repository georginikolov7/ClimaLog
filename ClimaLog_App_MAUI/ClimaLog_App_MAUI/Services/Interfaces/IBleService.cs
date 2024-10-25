using Plugin.BLE.Abstractions;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ClimaLog_App_MAUI.Services.Interfaces
{
    public interface IBleService
    {
        Task<DeviceState> ConnectToDeviceAsync();
        Task<DeviceState> DisconnectFromDeviceAsync();
        DeviceState GetConnectionState();
        Task<double> ReadCharacteristic();
        public Task SubscribeToCharacteristicUpdatesAsync(Action<int> onValueUdated);
        public Task UnsubscribeFromCharacteristicUpdatesAsync();
    }
}
