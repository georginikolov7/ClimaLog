using ClimaLog_Visualizer.Models;

namespace ClimaLog_Visualizer.Services
{
    public class InsideMeasurer : Measurer
    {
        public InsideMeasurer(double temperature, double humidity)
            : base(temperature, humidity) { }
        public string Name => "Inside Measurer";


        public override string Output => base.ToString();
    }
}
