using ClimaLog_App_MAUI.Services;
using ClimaLog_App_MAUI.Services.Interfaces;
using ClimaLog_App_MAUI.View;
using ClimaLog_App_MAUI.ViewModels;
using static ClimaLog_App_MAUI.Constants.AppConstants;

namespace ClimaLog_App_MAUI;

public static class MauiProgram
{
    public static MauiApp CreateMauiApp()
    {
        var builder = MauiApp.CreateBuilder();
        builder
            .UseMauiApp<App>()
            .ConfigureFonts(fonts =>
            {
                fonts.AddFont("OpenSans-Regular.ttf", "OpenSansRegular");
                fonts.AddFont("OpenSans-Semibold.ttf", "OpenSansSemibold");
            });
        builder.Services.AddSingleton<IBleService>(new BleService(DeviceName,Guid.Parse(serviceGuid),Guid.Parse(characteristicGuid)));
        builder.Services.AddSingleton<DataPage>();
        builder.Services.AddSingleton<SettingsPage>();
        builder.Services.AddSingleton<MeasurerService>();
        builder.Services.AddSingleton<MainPageViewModel>();
        builder.Services.AddSingleton<SettingsViewModel>();

        return builder.Build();
    }
}
