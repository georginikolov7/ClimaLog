namespace ClimaLog_App_MAUI.Models.Interfaces
{
    public interface IClimaReceivePacket
    {
        public DateTime ReceiveDate { get; }
        public IReadOnlyCollection<IMeasurer> Measurers { get; }
        public void AddMeasurer(IMeasurer measurer);
        public void SetReceiveDate(DateTime receiveDate);
    }
}
