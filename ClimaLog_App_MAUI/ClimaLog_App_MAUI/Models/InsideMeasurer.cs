
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ClimaLog_App_MAUI.Models
{
    public class InsideMeasurer : Measurer
    {


        public InsideMeasurer(string temperature, string humidity):base(temperature,humidity)
        {
            Name = "Inside";
            
        }

        public override string FormattedOutput => base.GetFormattedOutput();
    }
}
