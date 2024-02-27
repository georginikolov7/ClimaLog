using ClimaLog_App_MAUI.ViewModels;

namespace ClimaLog_App_MAUI.View;

public partial class DataPage : ContentPage
{
	public DataPage(MainPageViewModel viewModel)
	{
		InitializeComponent();
		BindingContext=viewModel;
	}
}