using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Windows.Media;
using Windows.Networking.Vpn;

namespace Wasapi.test
{
    [TestClass]
    public class AudioSessionCaptureClientTests
    {
        AudioSessionCaptureClient _sut;
        [TestInitialize]
        public async Task SetupClient()
        {
            _sut = await AudioSessionClient.CreateCaptureClientAsync();
            _sut.Initialize();
        }

        [TestCleanup]
        public void CloseClient()
        {
            _sut.Dispose();
        }


        [TestCategory("Properties")]
        [TestMethod]
        public void Capture_Period()
        {
            Assert.IsTrue(_sut.Period != 0);
        }
        [TestCategory("Properties")]
        [TestMethod]
        public void Capture_Format()
        {
            Assert.AreEqual("Audio", _sut.Format?.Type);
        }


        [TestCategory("Properties")]
        [TestMethod]
        public void Capture_BufferSize()
        {
            Assert.AreEqual(1056u, _sut.BufferSize);
        }

        [TestCategory("Properties")]
        [TestMethod]
        public void Capture_Latency()
        {
            Assert.AreEqual(TimeSpan.FromMilliseconds(0), _sut.Latency);
        }

        [TestCategory("Properties")]
        [TestMethod]
        public void Capture_Padding()
        {
            Assert.AreEqual(0u, _sut.Padding);
        }


        [TestCategory("Properties")]
        [TestMethod]
        public void Capture_NextPacketSize()
        {
            Assert.AreEqual(0u, _sut.NextPacketSize);
        }

        [TestCategory("Properties")]
        [TestMethod]
        public void Capture_GetFormat_TypeIsAudio()
        {
            var format = _sut.GetDefaultFormat();
            Assert.AreEqual("Audio", format.Type);
        }

        [TestCategory("Properties")]
        [TestMethod]
        public void Capture_GetFormat_SubTypeIsAudio()
        {
            var format = _sut.GetDefaultFormat();
            Assert.AreEqual("Float", format.Subtype);
        }

        [TestCategory("Properties")]
        [TestMethod]
        public void Capture_State_Running_After_Start()
        { 
            _sut.Start();
            Assert.AreEqual(AudioSessionClientState.Running, _sut.State);
        }

        [TestCategory("Properties")]
        [TestMethod]
        public void Capture_State_Stopped_After_Stop()
        {
            _sut.Start();
            _sut.Stop();
            Assert.AreEqual(AudioSessionClientState.Stopped, _sut.State);
        }

        [TestCategory("Operation")]
        [TestMethod]
        public void Capture_Start()
        {
            _sut.Start();
        }

        [TestCategory("Operation")]
        [TestMethod]
        public void Capture_Stop()
        {
            _sut.Start();
            _sut.Stop();
        }
        [TestCategory("Operation")]
        [TestMethod]
        public void Capture_Reset()
        {
            _sut.Reset();
        }

        [TestCategory("Operation")]
        [TestMethod]
        public void Capture_AddFrame()
        {
            var frame = _sut.GetFrame();
            Assert.IsNull(frame);
        }


    }

    [TestClass]
    public class AudioSessionCaptureClientCallbackTests : IAudioSessionCaptureCallback
    {
        AudioSessionCaptureClient _sut;
        List<TimeSpan> _callbacks;
        Stopwatch _sw;
        [TestInitialize]
        public async Task SetupClient()
        {
            _callbacks = new List<TimeSpan>();
            _sut = await AudioSessionClient.CreateCaptureClientAsync();
            _sut.Initialize(this);
        }

        [TestCleanup]
        public void CloseClient()
        {
            _sut.Dispose();
        }

        [TestCategory("Operation")]
        [TestMethod]
        public async Task Capture_Callbacks()
        {
            _sw = Stopwatch.StartNew();
            _sut.Start();
            await Task.Delay(50);
            _sut.Stop();
            Assert.IsTrue(_callbacks.Any());
        }

        public void OnFramesAvailable()
        {
            _callbacks.Add(_sw.Elapsed);
        }
    }
}
