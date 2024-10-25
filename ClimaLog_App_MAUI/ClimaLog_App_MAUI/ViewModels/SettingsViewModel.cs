using ClimaLog_App_MAUI.Helpers;
using ClimaLog_App_MAUI.Services.Interfaces;
using CommunityToolkit.Mvvm.Input;
using Plugin.BLE.Abstractions;
using System.ComponentModel;
using System.Windows.Input;
namespace ClimaLog_App_MAUI.ViewModels
{
    public partial class SettingsViewModel : INotifyPropertyChanged
    {

        private bool isBusy;
        private IBleService _bleService;
        private string connectionStatus;
        public SettingsViewModel(IBleService btService)
        {
            _bleService = btService;
        }
        public ICommand GetNetworksCommand { get; }
        public string ConnectionStatus
        {
            get => connectionStatus;
            set
            {
                connectionStatus = value;
                OnPropertyChanged(nameof(ConnectionStatus));
            }
        }
        public bool IsBusy
        {
            get => isBusy;
            set
            {
                if (isBusy == value)
                { return; }
                isBusy = value;
                OnPropertyChanged(nameof(IsBusy));
            }
        }
        public event PropertyChangedEventHandler PropertyChanged;
        public void OnPropertyChanged(string name)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
        }

        [RelayCommand]
        public async Task ReconnectDevice()
        {
            IsBusy = true;
            if (_bleService.GetConnectionState() == DeviceState.Connected)
            {
                await _bleService.DisconnectFromDeviceAsync();
            }
            await ConnectToBleDevice();
            IsBusy = false;
        }

        [RelayCommand]
        public async Task ConnectToBleDevice()
        {
            try
            {
                IsBusy = true;
                if (_bleService.GetConnectionState() == DeviceState.Connected)
                {
                    return;
                }
                ConnectionStatus = "Connecting...";

                var result = await _bleService.ConnectToDeviceAsync();
                if (result == DeviceState.Connected)
                {
                    ConnectionStatus = "Car connected";
                    
                }
                else
                {
                    ConnectionStatus = "Could not connect to car!";
                }
            }
            catch (PermissionException ex)
            {
                Console.WriteLine(ex.Message);
                await RequestBtPermission();
            }
            catch (InvalidOperationException ex)
            {
                ConnectionStatus = "Bluetooth Off!";
                Console.WriteLine(ex.Message);
                await Shell.Current.DisplayAlert("Bluetooth Off", "Turn on device bluetooth", "OK");
            }
            finally
            {
                IsBusy = false;
            }
        }

        [RelayCommand]
        public async Task RequestBtPermission()
        {
            var status = PermissionStatus.Unknown;
#if ANDROID
            status = await Permissions.RequestAsync<BlePermission>();
#elif IOS
            status = await Permissions.RequestAsync<Permissions.Bluetooth>();
#endif
            if (status == PermissionStatus.Granted)
            {
                return;
            }

            if (Permissions.ShouldShowRationale<BlePermission>())
            {
                await Shell.Current.DisplayAlert("Needs permission", "Permission required for establishing connection with H2 car bluetooth module", "OK");
            }
            if (status != PermissionStatus.Granted)
            {
                await Shell.Current.DisplayAlert("Permission required!", "Bluetooth permission is required for communication with H2 car", "OK");
            }
        }
    }
}
