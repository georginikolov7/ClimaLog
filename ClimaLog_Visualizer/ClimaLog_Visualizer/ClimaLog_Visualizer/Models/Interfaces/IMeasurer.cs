using System;
using System.Collections.Generic;
using System.Text;

namespace ClimaLog_Visualizer.Services.interfaces
{
    public interface IMeasurer
    {
        double Temperature { get; }
        double Humidity { get; }
        string Output { get; }
    }
}
