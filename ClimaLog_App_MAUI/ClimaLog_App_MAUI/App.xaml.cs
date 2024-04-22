using ClimaLog_App_MAUI.View;

namespace ClimaLog_App_MAUI;

public partial class App : Application
{
    public App()
    {
        InitializeComponent();

        MainPage = new AppShell();
    }
}
