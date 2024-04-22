using ClimaLog_App_MAUI.ViewModels;
using Xamarin.Essentials;
namespace ClimaLog_App_MAUI.View;

public partial class SettingsPage : ContentPage
{
    public SettingsPage(SettingsViewModel viewModel)
    {
        InitializeComponent();
        BindingContext = viewModel;
    }
    
}