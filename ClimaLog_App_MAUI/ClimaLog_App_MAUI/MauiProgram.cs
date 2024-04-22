using ClimaLog_App_MAUI.Services;
using ClimaLog_App_MAUI.Services.Interfaces;
using ClimaLog_App_MAUI.View;
using ClimaLog_App_MAUI.ViewModels;


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
		builder.Services.AddSingleton<DataPage>();
		builder.Services.AddSingleton<SettingsPage>();
		builder.Services.AddSingleton<MeasurerService>();
		builder.Services.AddSingleton<BtSerialService>();
		builder.Services.AddSingleton<MainPageViewModel>();
		builder.Services.AddSingleton<SettingsViewModel>();

		return builder.Build();
	}
}
