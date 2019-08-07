﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Devices.Enumeration;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using System.Linq;
using Wasapi;
using System.Threading.Tasks;
using Windows.Media.Devices;
using AudioVisualizer;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace WasapiSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page, IAudioSessionCaptureCallback
    {
        public MainPage()
        {
            this.InitializeComponent();
            InitializeAudioAsync();
        }


        AudioSessionCaptureClient audioClient;
        AudioAnalyzer analyzer;


        async void InitializeAudioAsync()
        {
            var defaultRenderDevice = await DeviceInformation.CreateFromIdAsync(Windows.Media.Devices.MediaDevice.GetDefaultAudioRenderId(AudioDeviceRole.Default));
            audioClient = await AudioSessionClient.CreateCaptureClientAsync(defaultRenderDevice);
            audioClient.InitializeLoopback(this);
            var format = audioClient.Format;
            var step = format.SampleRate / 60;
            analyzer = new AudioAnalyzer(100000, format.ChannelCount, format.SampleRate, step, 0, 2048, true);
         }


        private void BtnPlay_Click(object sender, RoutedEventArgs e)
        {
            audioClient.Start();
        }

        public void OnFramesAvailable()
        {
            var frame = audioClient.GetFrame();
            System.Diagnostics.Debug.WriteLine(frame.Duration);
            analyzer.ProcessInput(frame);
        }

        private void BtnStart_Click(object sender, RoutedEventArgs e)
        {
            audioClient.Start();
        }
    }
}
