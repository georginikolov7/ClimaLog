using ClimaLog_App_MAUI.Models.Interfaces;

namespace ClimaLog_App_MAUI.Services.Interfaces
{
    public interface IMeasurerService
    {
        Task<IClimaReceivePacket> GetMeasurersData();
    }
}