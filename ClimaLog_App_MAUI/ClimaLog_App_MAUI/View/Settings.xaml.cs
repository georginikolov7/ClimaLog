using ClimaLog_App_MAUI.ViewModels;
namespace ClimaLog_App_MAUI.View;

public partial class SettingsPage : ContentPage
{
    private readonly SettingsViewModel viewModel;
    public SettingsPage(SettingsViewModel viewModel)
    {
        InitializeComponent();
        BindingContext = viewModel;
        this.viewModel = viewModel;
    }
    protected override async void OnAppearing()
    {
        base.OnAppearing();
        await viewModel.RequestBtPermission();
    }

}