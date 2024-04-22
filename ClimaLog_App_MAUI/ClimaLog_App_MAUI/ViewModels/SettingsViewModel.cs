using ClimaLog_App_MAUI.Helpers;
using ClimaLog_App_MAUI.Services;
using CommunityToolkit.Mvvm.Input;
using System.ComponentModel;
using System.Windows.Input;

namespace ClimaLog_App_MAUI.ViewModels
{
    public partial class SettingsViewModel : INotifyPropertyChanged
    {

        private const string esp32Name = "ESP32-BT_HEHE";

        private bool isBusy;
        private BtSerialService btSerial;
        private string connectionStatus;
        public SettingsViewModel(BtSerialService btService)
        {
            btSerial = btService;
            ConnectCommand = new Command(async () => await ConnectToDevice());
        }
        public ICommand getNetworksCommand { get; }
        public ICommand ConnectCommand { get; }
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
        private async Task ConnectToDevice()
        {
            // Implement connection logic
            bool isConnected = await btSerial.ConnectAsync(esp32Name);
            if (isConnected)
            {
                connectionStatus = "Connected successfully";
            }
        }

        [RelayCommand]
        async Task RequestBt()
        {
            var status = PermissionStatus.Unknown;
            status = await Permissions.CheckStatusAsync<BtPermission>();
            if (status == PermissionStatus.Granted)
            {
                return;
            }
            if (Permissions.ShouldShowRationale<BtPermission>())
            {
                await Shell.Current.DisplayAlert("Needs permission", "OTI SHO PROBLEM LI IMA", "OK");
            }
            status = await Permissions.RequestAsync<BtPermission>();

            if (status != PermissionStatus.Granted)
            {
                await Shell.Current.DisplayAlert("Permission required!", "Bluetooth permission is required for communication with ESP32", "OK");
            }
        }
    }
}
