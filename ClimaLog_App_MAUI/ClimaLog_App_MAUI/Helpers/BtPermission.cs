using static Microsoft.Maui.ApplicationModel.Permissions;
namespace ClimaLog_App_MAUI.Helpers
{
    public class BtPermission : BasePlatformPermission
    {
        public override (string androidPermission, bool isRuntime)[] RequiredPermissions => new List<(string permission, bool isRuntime)>
        {
            ("android.permission.BLUETOOTH",true),
            ("android.permission.BLUETOOTH_SCAN",true),
            ("android.permission.BLUETOOTH_CONNECT",true),
            ("android.permission.BLUETOOTH_ADMIN",true),
            ("android.permission.ACCESS_BACKGROUND_LOCATION",true)
        }.ToArray();

    }
}