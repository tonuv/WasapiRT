using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Windows.Media.MediaProperties;

namespace Wasapi.test
{
    [TestClass]
    public class AudioSessionRenderClientUninitializedTests : IAudioSessionRenderCallback
    {
        AudioSessionRenderClient _sut;
        [TestInitialize]
        public async Task ActivateClient()
        {
            _sut = await AudioSessionClient.CreateRenderClientAsync();
        }
        [TestCleanup]
        public void CloseClient()
        {
            _sut.Dispose();
        }

        [TestCategory("Properties")]
        [TestMethod]
        public void Render_Uninitialized_BufferSize_Throws()
        {
            var result = Assert.ThrowsException<COMException>(
                () =>
                {
                    var bufferSize = _sut.BufferSize;
                });

            Assert.AreEqual(WasapiErrors.AUDCLNT_E_NOT_INITIALIZED, result.HResult);
        }

        [TestCategory("Properties")]
        [TestMethod]
        public void Render_Uninitialized_DefaultPeriod()
        {
            var defaultPeriod = _sut.DefaultPeriod;
            Assert.AreEqual(TimeSpan.FromMilliseconds(10), defaultPeriod);
        }
        [TestCategory("Properties")]
        [TestMethod]
        public void Render_Uninitialized_MinimumPeriod()
        {
            var minimumPeriod = _sut.MinimumPeriod;
            Assert.AreEqual(TimeSpan.FromMilliseconds(3), minimumPeriod);
        }


        [TestCategory("Properties")]
        [TestMethod]
        public void Render_Uninitialized_GetFormat()
        {
            var format = _sut.GetFormat();
            Assert.AreEqual(format.Type, "Audio");
        }

        [TestCategory("Properties")]
        [TestMethod]
        public void Render_Uninitialized_IsFormatSupported_PCM()
        {
            var format = AudioEncodingProperties.CreatePcm(48000, 2, 32);
            format.Subtype = "Float";
            Assert.IsTrue(_sut.IsFormatSupported(format));
        }
        [TestCategory("Properties")]
        [TestMethod]
        public void Render_Uninitialized_IsFormatSupported_MP3()
        {
            var format = AudioEncodingProperties.CreateMp3(48000, 2, 192000);
            Assert.IsFalse(_sut.IsFormatSupported(format));
        }


        [TestCategory("Properties")]
        [TestMethod]
        public void Render_Uninitialized_Start_Throws()
        {
            var result = Assert.ThrowsException<COMException>(
                () =>
                {
                    _sut.Start();
                });

            Assert.AreEqual(WasapiErrors.AUDCLNT_E_NOT_INITIALIZED, result.HResult);
        }
        [TestCategory("Properties")]
        [TestMethod]
        public void Render_Uninitialized_Stop_Throws()
        {
            var result = Assert.ThrowsException<COMException>(
                () =>
                {
                    _sut.Stop();
                });

            Assert.AreEqual(WasapiErrors.AUDCLNT_E_NOT_INITIALIZED, result.HResult);
        }
        [TestCategory("Properties")]
        [TestMethod]
        public void Render_Uninitialized_Reset_Throws()
        {
            var result = Assert.ThrowsException<COMException>(
                () =>
                {
                    _sut.Reset();
                });

            Assert.AreEqual(WasapiErrors.AUDCLNT_E_NOT_INITIALIZED, result.HResult);
        }

        [TestCategory("Initialization")]
        [TestMethod]
        public void Render_InitializeWithDefaults()
        {
            _sut.Initialize();
        }
        [TestCategory("Initialization")]
        [TestMethod]
        public void Render_InitializeEventDrivenWithDefaults()
        {
            _sut.Initialize(this);
        }

        public void OnFramesNeeded()
        {
            throw new NotImplementedException();
        }
    }

}
