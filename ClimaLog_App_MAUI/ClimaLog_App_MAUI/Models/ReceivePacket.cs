using ClimaLog_App_MAUI.Models.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace ClimaLog_App_MAUI.Models
{
    public class ReceivePacket : IClimaReceivePacket
    {
        private List<IMeasurer> measurers;
        public ReceivePacket()
        {
            measurers = new List<IMeasurer>();
        }
        public DateTime ReceiveDate { get; private set; }

        public IReadOnlyCollection<IMeasurer> Measurers => measurers.AsReadOnly();
        public void AddMeasurer(IMeasurer measurer)
        {
            if (measurer != null)
            {
                measurers.Add(measurer);
            }
            else
            {
                throw new ArgumentException("Cannot add null object to measurer list");
            }
        }

        public void SetReceiveDate(DateTime receiveDate)
        {
            ReceiveDate = receiveDate;
        }
    }
}
