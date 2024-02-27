using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ClimaLog_App_MAUI.Models
{
    public class OutsideMeasurer : Measurer
    {
        private string snow;
        private string battery;
        private int index;

        public int Index
        {
            get { return index; }
            set
            {
                if (value < 0)
                {
                    throw new ArgumentOutOfRangeException("Index of module cannot be negative");
                }
                index = value;
            }
        }

        public OutsideMeasurer(string temperature, string humidity, string snow, string battery, int index) : base(temperature, humidity)
        {
            Snow = snow;
            Battery = battery;
            Name = "Outside " + index;
        }
        public string Battery
        {
            get { return battery; }
            set { battery = value; }
        }

        public string Snow
        {
            get { return snow; }
            set { snow = value; }
        }

        public override string FormattedOutput => this.GetFormattedOutput();

        public override string GetFormattedOutput()
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine(base.GetFormattedOutput());
            sb.AppendLine($"{nameof(Snow)}: {Snow} cm");
            sb.AppendLine($"{nameof(Battery)}: {Battery} %");
            return sb.ToString().Trim();
        }
    }
}
