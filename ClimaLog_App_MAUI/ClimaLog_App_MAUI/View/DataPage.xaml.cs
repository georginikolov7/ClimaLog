using ClimaLog_App_MAUI.ViewModels;

namespace ClimaLog_App_MAUI.View;

public partial class DataPage : ContentPage
{
    private readonly MainPageViewModel vm;
    public DataPage(MainPageViewModel viewModel)
    {
        InitializeComponent();
        BindingContext = viewModel;
        vm = viewModel;
    }
    protected override async void OnAppearing()
    {
        await vm.GetMeasurersAsync();
        base.OnAppearing();
    }
}