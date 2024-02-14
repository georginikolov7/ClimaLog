using ClimaLog_Visualizer.Models;
using ClimaLog_Visualizer.Services.interfaces;
using System;
using System.Collections.Generic;
using System.Text;

namespace ClimaLog_Visualizer.Services
{
    public class InsideMeasurer : Measurer
    {

        public InsideMeasurer(double temp, int hum) : base(temp, hum) { }
        public string Name => "Inside Measurer";
        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine(Name);
            sb.AppendLine(base.ToString());
            return sb.ToString().Trim();
        }

    }
}
