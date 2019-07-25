using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

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
            Assert.AreEqual(1056u, _sut.Padding);
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
            _sut.Start();
        }


    }
}
