using ClimaLog_Visualizer.Services.interfaces;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Net.Http;
using Xamarin.Forms;
using Newtonsoft.Json;
namespace ClimaLog_Visualizer
{
    public partial class MainPage : ContentPage
    {
        private string googleScriptURL = @"https://script.google.com/macros/s/AKfycbwPVQj9ozgZ-AVm4ROIXJ0XxymcB2ekPrGLKMVVQOsciNQpWbbY9tSoP_mLMrJI1u4Vgg/exec";
        public IList<IMeasurer> Measurers { get; private set; }
        public DateTime MeasureDate { get; private set; }
        public MainPage()
        {
            InitializeComponent();
            BindingContext = this;

        }
        protected override void OnAppearing()
        {
            base.OnAppearing();
            SendDoGetRequestToGoogle();

        }
        private void AppendData(string JSON)
        {
            //Split the JSON manually:
            //{"Date":"2024-02-11T07:00:02.656Z","InsideTemperature":21.6,"InsideHumidity":51,"OutsideTemperature":4.2,"OutsideHumidity":68,"OutsideSnow depth":5,"OutsideBattery level":79}
            object obj = JsonConvert.DeserializeObject(JSON);
            Debug.WriteLine(obj);
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
    }
}
