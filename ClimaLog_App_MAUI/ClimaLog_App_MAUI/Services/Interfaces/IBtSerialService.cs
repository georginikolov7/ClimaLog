using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ClimaLog_App_MAUI.Services.Interfaces
{
    public interface IBtSerialService
    {
        public Task<bool> ConnectAsync(string deviceName);
        public Task SendAsync(string message);
        public Task<string> ReceiveMessageAsync();
    }
}
