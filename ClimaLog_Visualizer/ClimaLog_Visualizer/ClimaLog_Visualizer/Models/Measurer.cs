using ClimaLog_Visualizer.Services.interfaces;
using System;
using System.Collections.Generic;
using System.Text;

namespace ClimaLog_Visualizer.Models
{
    public abstract class Measurer : IMeasurer
    {
        private double temperature;
        private double humidity;
        public Measurer(double temp, double hum)
        {
            Temperature = temp;
            Humidity = hum;
        }
        public virtual double Temperature
        {
            get => temperature; private set
            {
                if (value > -30 && value < 45)
                {
                    temperature = value;
                }
            }
        }
        public virtual double Humidity
        {
            get => humidity; private set
            {
                if (value >= 0 && value <= 100)
                {
                    humidity = value;
                }
            }
        }

        public abstract string Output { get; }


        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine($"{nameof(Temperature)}: {Temperature} \u2103");
            sb.AppendLine($"{nameof(Humidity)}: {Humidity} %");
            return sb.ToString().Trim();
        }
    }
}
