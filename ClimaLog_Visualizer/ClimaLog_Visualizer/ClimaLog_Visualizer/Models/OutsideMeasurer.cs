using ClimaLog_Visualizer.Services.interfaces;
using System;
using System.Collections.Generic;
using System.Text;

namespace ClimaLog_Visualizer.Models
{
    public class OutsideMeasurer : Measurer
    {
        int snowDepth;
        int batteryLevel;
        private int index;
        public OutsideMeasurer(int index, double temperature, double humidity, int snowDepth, int batPercentage) : base(temperature, humidity)
        {
            Index = index;
            SnowDepth = snowDepth;
            BatteryLevel = batPercentage;
        }

        public int Index
        {
            get { return index; }
            set { index = value; }
        }

        public int SnowDepth
        {
            get => snowDepth;
            set
            {
                if (value >= 0 || value < 110)
                {
                    snowDepth = value;
                }
            }
        }
        public int BatteryLevel
        {
            get => batteryLevel;
            set
            {
                if (value >= 0 || value <= 100)
                {
                    batteryLevel = value;
                }
            }
        }
        public string Name => "Outside Measurer " + Index;
        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine(Name);
            sb.AppendLine(base.ToString());
            sb.AppendLine($"{nameof(SnowDepth)}: {snowDepth} cm");
            sb.AppendLine($"{nameof(BatteryLevel)}: {BatteryLevel} %");
            return base.ToString().Trim();
        }
    }
}
