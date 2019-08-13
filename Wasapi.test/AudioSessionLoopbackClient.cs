using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Media;
using Windows.Media.Devices;
using Windows.Networking.Vpn;

namespace Wasapi.test
{

    [TestClass]
    public class AudioSessionLoopbackCallbackTests : IAudioSessionCaptureCallback
    {
        AudioSessionCaptureClient _sut;
        List<TimeSpan> _callbacks;
        Stopwatch _sw;
        [TestInitialize]
        public async Task SetupClient()
        {
            _callbacks = new List<TimeSpan>();
            var defaultRenderDevice = await DeviceInformation.CreateFromIdAsync(MediaDevice.GetDefaultAudioRenderId(AudioDeviceRole.Default));
            _sut = await AudioSessionClient.CreateCaptureClientAsync(defaultRenderDevice);
            _sut.InitializeLoopback(this);
        }

        [TestCleanup]
        public void CloseClient()
        {
            _sut.Dispose();
        }

        [TestCategory("Operation")]
        [TestMethod]
        public async Task Loopback_Callbacks()
        {
            _sw = Stopwatch.StartNew();
            _sut.Start();
            await Task.Delay(250);
            _sut.Stop();
            Assert.IsTrue(_callbacks.Any());
        }

        public void OnFramesAvailable()
        {
            _callbacks.Add(_sw.Elapsed);
        }
    }
}
