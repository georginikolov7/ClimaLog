
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ClimaLog_App_MAUI.Models.Interfaces
{
    public interface IMeasurer
    {
        string FormattedOutput { get; }
        string Name { get; }

    }
}
