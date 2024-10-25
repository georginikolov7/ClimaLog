using ClimaLog_App_MAUI.Models.Interfaces;
using ClimaLog_App_MAUI.Services;
using CommunityToolkit.Mvvm.Input;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Windows.Input;

namespace ClimaLog_App_MAUI.ViewModels
{
    public partial class MainPageViewModel : INotifyPropertyChanged
    {
        private bool isBusy;
        private bool isRefreshing;
        private bool isDataVisible;
        private MeasurerService measurerService;
        private DateTime receiveDate;
        public MainPageViewModel(MeasurerService measurerService)
        {
            this.measurerService = measurerService;

        }
        public DateTime ReceiveDate
        {
            get => receiveDate;
            set
            {
                receiveDate = value;
                OnPropertyChanged(nameof(ReceiveDate));
            }
        }
        public ObservableCollection<IMeasurer> Measurers { get; } = new();
        public bool IsRefreshing
        {
            get => isRefreshing;
            set
            {
                if (isRefreshing != value)
                {
                    isRefreshing = value;
                    OnPropertyChanged(nameof(IsRefreshing));
                }
            }
        }
        public bool IsDataVisible
        {
            get => isDataVisible; set
            {
                isDataVisible = value;
                OnPropertyChanged(nameof(IsDataVisible));
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
        public async Task GetMeasurersAsync()
        {
            IsDataVisible = false;
            if (isBusy)
            {
                return;
            }
            try
            {
                isBusy = true;

                if (Measurers.Count != 0)
                {
                    Measurers.Clear();
                }
                IClimaReceivePacket climaReceivePacket = await measurerService.GetMeasurersData();
                ReceiveDate = climaReceivePacket.ReceiveDate;
                foreach (IMeasurer measurer in climaReceivePacket.Measurers)
                {
                    Measurers.Add(measurer);
                }
            }
            catch (Exception ex)
            {
                await Shell.Current.DisplayAlert("Error occurred", $"Unable to obtain data: {ex.Message}", "OK");
                Debug.WriteLine(ex);
            }
            finally
            {
                isBusy = false;
                IsRefreshing = false;
                IsDataVisible = true;
            }

        }
        [RelayCommand]
        public async Task Refresh()
        {
            await GetMeasurersAsync();
        }
    }
}

