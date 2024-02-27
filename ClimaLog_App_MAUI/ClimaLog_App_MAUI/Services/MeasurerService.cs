using ClimaLog_App_MAUI.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Http;
using System.Net.Http.Json;
using Newtonsoft.Json;
using ClimaLog_App_MAUI.Models.Interfaces;

namespace ClimaLog_App_MAUI.Services
{
    public class MeasurerService
    {
        private HttpClient httpClient;
        private const string URL = @"https://script.google.com/macros/s/AKfycbzLNMP0ekthwSC75XHSm_SIWzqAxo71Y18XLp1xA2tMTEhDXsM-43s7P0pWbY0X50lo/exec";
        private IClimaReceivePacket dataPacket;
        public MeasurerService()
        {
            httpClient = new HttpClient();
        }
        public async Task<IClimaReceivePacket> GetMeasurersData()
        {
            dataPacket = new ReceivePacket();
            if (dataPacket.Measurers?.Count > 0)
            {
                return dataPacket;
            }
            var response = await httpClient.GetAsync(URL);
            if (response.IsSuccessStatusCode)
            {
                string mainJSON = await response.Content.ReadAsStringAsync();
                dataPacket = DeserializeMeasurersJSON(mainJSON);
                return dataPacket;
            }
            else
            {
                //couldn't fetch data:

                return null;
            }

        }
        private IClimaReceivePacket DeserializeMeasurersJSON(string json)
        {
            IClimaReceivePacket packet = new ReceivePacket();

            Dictionary<string, string> mainJSON = JsonConvert.DeserializeObject<Dictionary<string, string>>(json);
            foreach (var kvp in mainJSON)
            {
                switch (kvp.Key.Split()[0])
                {
                    case "Inside":
                        packet.AddMeasurer(JsonConvert.DeserializeObject<InsideMeasurer>(kvp.Value));
                        break;
                    case "Outside":
                        packet.AddMeasurer(JsonConvert.DeserializeObject<OutsideMeasurer>(kvp.Value));
                        break;
                    case "Date":
                        string dateString = mainJSON["Date"];
                        packet.SetReceiveDate(DateTime.Parse(dateString));
                        break;
                    default:
                        throw new ArgumentException("Invalid argument in JSON");
                }
            }
            return packet;
        }
    }
}
