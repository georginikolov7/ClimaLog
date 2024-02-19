using ClimaLog_Visualizer.Services.interfaces;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Net.Http;
using Xamarin.Forms;
using Newtonsoft.Json;
using System.ComponentModel;
using ClimaLog_Visualizer.Services;
using ClimaLog_Visualizer.Models;
using System.Linq;

namespace ClimaLog_Visualizer
{


    //TODO:
    //1) Add refresh option + animation
    //2) Error handling
    public partial class MainPage : ContentPage, INotifyPropertyChanged
    {
        private const string googleScriptURL = @"https://script.google.com/macros/s/AKfycbxOXIwKKBMgATxoWHVs-h-vnN64kPiSi1YzX-6Rf5EsbYpFD05Ggv6QYY4OG6dHRMeA/exec";
        private DateTime date;
        public IList<IMeasurer> Measurers
        {
            get; private set;
        }
        public DateTime MeasureDate
        {
            get => date; private set
            {
                date = value;
                OnPropertyChanged(nameof(MeasureDate));
            }
        }
        public MainPage()
        {
            InitializeComponent();
            MeasureDate = DateTime.Now;
            BindingContext = this;

        }
        protected override void OnAppearing()
        {
            base.OnAppearing();
            SendDoGetRequestToGoogle();

        }
        private void AppendData(string JSON)
        {
            

            //{"Date":"2024-02-11T07:00:02.656Z","Inside":"{\"Temperature\":21.6,\"Humidity\":51}","Outside 1":"{\"Temperature\":4.2,\"Humidity\":68,\"Snow depth\":5,\"Battery level\":79}"}

            //main JSON
            Dictionary<string, string> mainJSON = JsonConvert.DeserializeObject<Dictionary<string, string>>(JSON);

            Measurers = new List<IMeasurer>();

            foreach (var kvp in mainJSON)
            {
                switch (kvp.Key.Split()[0])
                {
                    case "Inside":
                        Measurers.Add(JsonConvert.DeserializeObject<InsideMeasurer>(kvp.Value));
                        break;
                    case "Outside":
                        Measurers.Add(JsonConvert.DeserializeObject<OutsideMeasurer>(kvp.Value));
                        break;
                    case "Date":
                        string dateString = mainJSON["Date"];
                        MeasureDate = DateTime.Parse(dateString);
                        break;
                    default:
                        throw new ArgumentException("Invalid argument in JSON");
                }
            }
            OnPropertyChanged(nameof(Measurers));
        }
        private async void SendDoGetRequestToGoogle()
        {
            using (HttpClient client = new HttpClient())
            {
                HttpResponseMessage response = await client.GetAsync(googleScriptURL);
                if (response.IsSuccessStatusCode)
                {
                    // Read and parse JSON response
                    string json = await response.Content.ReadAsStringAsync();
                    Debug.WriteLine(json);
                    AppendData(json);

                }
                else
                {
                    //HTTPS error:
                    //To do Error Handling
                }
            }


        }
        protected void OnPropertyChanged(string propertyName)
        {
            // Notify the UI of property changes
            OnPropertyChanged(new PropertyChangedEventArgs(propertyName));
        }

        protected void OnPropertyChanged(PropertyChangedEventArgs e)
        {
            // Notify the UI of property changes
            PropertyChanged?.Invoke(this, e);
        }

        public event PropertyChangedEventHandler PropertyChanged;
    }
}
