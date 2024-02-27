using ClimaLog_App_MAUI.Models;
using System.Text;

namespace TestProject1
{
    public class Tests
    {
        private InsideMeasurer measurer;
        [SetUp]
        public void Setup()
        {
            measurer = new("25", "40");

        }

        [Test]
        public void Constructor_ShouldCreate_Instance()
        {
            measurer = new("25", "40");

            Assert.IsNotNull(measurer);
        }
        [Test]
        public void InsideMeasurerOutputShouldBeCorrect()
        {
            StringBuilder expectedOutput = new StringBuilder();
            expectedOutput.AppendLine("Temperature: 25 ℃");
            expectedOutput.AppendLine("Humidity: 40 %");
            Assert.AreEqual(expectedOutput.ToString().Trim(), measurer.FormattedOutput);


        }
    }
}