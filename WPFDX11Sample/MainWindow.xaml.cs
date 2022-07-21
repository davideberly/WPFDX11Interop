// The MIT License (MIT)
//
// Copyright (c) 2015 Microsoft
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// Note from Geometric Tools. This file is a modification of the one provided
// by the Microsoft github project for WPF-DX10/11 interoperability.

using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Interop.DirectX;
using System.Windows.Media;

namespace WPFDX11Sample
{
    public partial class MainWindow : Window
    {
        private readonly DX11Managed dx11Manager;
        private readonly RenderFrameTimer timer;
        private TimeSpan lastRender;
        private bool lastVisible;
        public MainWindow()
        {
            // After the construction call, an exception occurred when
            // dx11Manager.exceptionMessage is not "".
            dx11Manager = new DX11Managed();

            timer = new RenderFrameTimer(30);

            InitializeComponent();
            CompositionTarget.Rendering += new EventHandler(OnComposition);
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            this.d3d11Image!.WindowOwner = (new System.Windows.Interop.WindowInteropHelper(this)).Handle;
            this.d3d11Image.OnRender = this.DoRender;
            this.d3d11Image.RequestRender();

            timer.Reset();
        }

        void OnComposition(object? sender, EventArgs? e)
        {
            RenderingEventArgs args = (RenderingEventArgs)e!;

            if (this.d3d11Image!.IsFrontBufferAvailable &&
                this.lastRender != args.RenderingTime)
            {
                this.d3d11Image.RequestRender();
                this.lastRender = args.RenderingTime;
            }
        }
        private void DoRender(IntPtr wpfBackBuffer, bool recreateRenderTarget)
        {
            timer.Measure();

            // If the RenderFrame call returns 'false', an exception occurred.
            // The message is described by dx11Manager.exceptionMessage.
            _ = dx11Manager.RenderFrame(wpfBackBuffer, recreateRenderTarget);

            double rate = timer.GetFramesPerSecond();
            this.textbox.Text = "fps = " + rate.ToString("F1");
            timer.UpdateFrameCount();
        }
        private void OnClosing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            CompositionTarget.Rendering -= this.OnComposition;
            this.dx11Manager?.Dispose();
            this.d3d11Image?.Dispose();
        }
        private void OnSizeChanged(object sender, SizeChangedEventArgs e)
        {
            uint xSize = 0, ySize = 0;
            GetSize(ref xSize, ref ySize);

            this.d3d11Image.Resize(xSize, ySize);

            bool isVisible = (xSize != 0 && ySize != 0);
            if (lastVisible != isVisible)
            {
                lastVisible = isVisible;
                if (lastVisible)
                {
                    CompositionTarget.Rendering += this.OnComposition;
                }
                else
                {
                    CompositionTarget.Rendering -= this.OnComposition;
                }
            }
        }

        private void GetSize(ref uint xSize, ref uint ySize)
        {
            double dpiScale = 1.0;
            if (PresentationSource.FromVisual(this).CompositionTarget is HwndTarget hwndTarget)
            {
                dpiScale = hwndTarget.TransformToDevice.M11;
            }

            xSize = (uint)(Math.Ceiling(host.ActualWidth * dpiScale));
            ySize = (uint)(Math.Ceiling(host.ActualHeight * dpiScale));
        }

        private void OnKeyDown(object sender, System.Windows.Input.KeyEventArgs e)
        {
            if (e.Key == System.Windows.Input.Key.Space)
            {
                timer.Reset();
            }
        }
    }
}
