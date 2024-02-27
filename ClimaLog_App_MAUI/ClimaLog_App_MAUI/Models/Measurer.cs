using ClimaLog_App_MAUI.Models.Interfaces;
using System.Text;

namespace ClimaLog_App_MAUI.Models
{
    public abstract class Measurer : IMeasurer
    {
        private string temperature;
        private string humidity;
        private string name;
        public Measurer(string temperature,string humidity)
        {
            Temperature = temperature;
            Humidity = humidity;
        }
        public string Name
        {
            get => name;
            protected set
            {
                if (string.IsNullOrWhiteSpace(value))
                {
                    throw new ArgumentException("Name cannot be null or empty");
                }
                name = value;
            }
        }

        public string Humidity
        {
            get => humidity;
            private set { humidity = value; }
        }

        public string Temperature
        {
            get => temperature;
            private set { temperature = value; }
        }

        public abstract string FormattedOutput { get; }

        public virtual string GetFormattedOutput()
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine($"{nameof(Temperature)}: {Temperature} \u2103");
            sb.AppendLine($"{nameof(Humidity)}: {Humidity} %");
            return sb.ToString().Trim();
        }


    }
}
