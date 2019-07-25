
using System;
using System.Threading.Tasks;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Wasapi;
using Windows.Devices.Enumeration;
using Windows.Networking.Vpn;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using Microsoft.VisualStudio.TestTools.UnitTesting.AppContainer;
using Windows.Media.MediaProperties;

namespace Wasapi.test
{
    [TestClass]
    public class AudioSessionClientActivation
    {
        [TestMethod]
        [TestCategory("Activation")]
        public async Task CreateRenderAsync()
        {
            var client = await AudioSessionClient.CreateRenderClientAsync();
            Assert.IsNotNull(client);
        }
        [TestMethod]
        [TestCategory("Activation")]
        public async Task CreateRenderWithDeviceAsync()
        {
            var renderDevices = await DeviceInformation.FindAllAsync(DeviceClass.AudioRender);
            if (!renderDevices.Any())
            {
                Assert.Inconclusive("No render device found");
            }
            else
            {
                var client = await AudioSessionClient.CreateRenderClientAsync(renderDevices.First());
                Assert.IsNotNull(client);
            }
        }

        // As per docs need to call capture activation on UI thread
        [TestMethod]
        [TestCategory("Activation")]
        public async Task CreateCaptureAsync()
        {
            var client = await AudioSessionClient.CreateCaptureClientAsync();
            Assert.IsNotNull(client);
        }
        [TestMethod]
        [TestCategory("Activation")]
        public async Task CreateCaptureWithDeviceAsync()
        {
            var captureDevices = await DeviceInformation.FindAllAsync(DeviceClass.AudioCapture);
            if (!captureDevices.Any())
            {
                Assert.Inconclusive("No render device found");
            }
            else
            {
                var client = await AudioSessionClient.CreateCaptureClientAsync(captureDevices.First());
                Assert.IsNotNull(client);
            }
        }

    }
}
