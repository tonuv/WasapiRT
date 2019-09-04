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
    public class AudioSessionRenderClientTests
    {
        AudioSessionRenderClient _sut;
        [TestInitialize]
        public async Task SetupClient()
        {
            _sut = await AudioSessionClient.CreateRenderClientAsync();
            _sut.Initialize();
        }

        [TestCleanup]
        public void CloseClient()
        {
            _sut.Dispose();
        }

        [TestCategory("Properties")]
        [TestMethod]
        public void Render_Period()
        {
            Assert.IsTrue(_sut.Period != 0);
        }
        [TestCategory("Properties")]
        [TestMethod]
        public void Render_Format()
        {
            Assert.AreEqual("Audio", _sut.Format?.Type);
        }

        [TestCategory("Properties")]
        [TestMethod]
        public void Render_BufferSize()
        {
            Assert.AreEqual(1056u, _sut.BufferSize);
        }

        [TestCategory("Properties")]
        [TestMethod]
        public void Render_Latency()
        {
            Assert.AreEqual(TimeSpan.FromMilliseconds(0), _sut.Latency);
        }

        [TestCategory("Properties")]
        [TestMethod]
        public void Render_Padding()
        {
            Assert.AreEqual(0u, _sut.Padding);
        }

        [TestCategory("Properties")]
        [TestMethod]
        public void Render_FramesNeeded()
        {
            Assert.AreEqual(_sut.BufferSize, _sut.FramesNeeded);
        }

        [TestCategory("Properties")]
        [TestMethod]
        public void Render_GetDefaultFormat_TypeIsAudio()
        {
            var format = _sut.GetDefaultFormat();
            Assert.AreEqual("Audio", format.Type);
        }


        [TestCategory("Properties")]
        [TestMethod]
        public void Render_State_Running_After_Start()
        {
            _sut.Start();
            Assert.AreEqual(AudioSessionClientState.Running, _sut.State);
        }

        [TestCategory("Properties")]
        [TestMethod]
        public void Render_State_Stopped_After_Stop()
        {
            _sut.Start();
            _sut.Stop();
            Assert.AreEqual(AudioSessionClientState.Stopped, _sut.State);
        }


        [TestCategory("Operation")]
        [TestMethod]
        public void Render_Start()
        {
            _sut.Start();
        }

        [TestCategory("Operation")]
        [TestMethod]
        public void Render_Stop()
        {
            _sut.Start();
            _sut.Stop();
        }
        [TestCategory("Operation")]
        [TestMethod]
        public void Render_Reset()
        {
            _sut.Reset();
        }

        [TestCategory("Operation")]
        [TestMethod]
        public void Render_AddFrame()
        {
            var frame = new AudioFrame(_sut.BufferSize * 4);
            using (var buffer = frame.LockBuffer(AudioBufferAccessMode.Read))
            {
                Assert.AreEqual(_sut.FramesNeeded, _sut.BufferSize);
                _sut.AddFrames(buffer);
                Assert.AreEqual(_sut.FramesNeeded, 0u);
            }
        }


    }

    [TestClass]
    public class AudioSessionRenderClientCallbackTests : IAudioSessionRenderCallback
    {
        AudioSessionRenderClient _sut;
        List<TimeSpan> _callbacks;
        Stopwatch _sw;
        [TestInitialize]
        public async Task SetupClient()
        {
            _callbacks = new List<TimeSpan>();
            _sut = await AudioSessionClient.CreateRenderClientAsync();
            var period = _sut.GetPeriods(_sut.GetDefaultFormat()).DefaultPeriodFrames;
            _sut.Initialize(this);
        }

        [TestCleanup]
        public void CloseClient()
        {
            _sut.Dispose();
        }

        [TestCategory("Operation")]
        [TestMethod]
        public async Task Render_Callbacks()
        {
            _sw = Stopwatch.StartNew();
            _sut.Start();
            await Task.Delay(50);
            _sut.Stop();
            Assert.IsTrue(_callbacks.Any());
        }

        public void OnFramesNeeded()
        {
            _callbacks.Add(_sw.Elapsed);
        }
    }
}
