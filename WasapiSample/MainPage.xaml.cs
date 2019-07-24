using System;
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

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace WasapiSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page, IAudioSessionRenderCallback
    {
        public MainPage()
        {
            this.InitializeComponent();
            InitializeRenderAsync();
        }


        AudioSessionRenderClient audioClient;
        async void InitializeRenderAsync()
        {
            audioClient = await AudioSessionClient.CreateRenderClientAsync();
            audioClient.Initialize(this);
         }

        public void OnFramesNeeded()
        {
            System.Diagnostics.Debug.WriteLine($"Callback {DateTime.Now}");
        }

        private void BtnPlay_Click(object sender, RoutedEventArgs e)
        {
            audioClient.Start();
        }
    }
}
