using ClimaLog_App_MAUI.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TestProject1
{
    public class OutsideMeasurerTests
    {
        private OutsideMeasurer measurer;
        [SetUp]
        public void Setup()
        {
            measurer = new("25", "40","100","45",1);

        }

        [Test]
        public void Constructor_ShouldCreate_Instance()
        {
            Assert.IsNotNull(measurer);
        }
        [Test]
        public void Output_ShouldBeCorrect()
        {
            StringBuilder expectedOutput = new StringBuilder();
            expectedOutput.AppendLine("Temperature: 25 ℃");
            expectedOutput.AppendLine("Humidity: 40 %");
            expectedOutput.AppendLine("Snow: 100 cm");
            expectedOutput.AppendLine("Battery: 45 %");
            Assert.AreEqual(expectedOutput.ToString().Trim(), measurer.FormattedOutput);
        }
    }
}
