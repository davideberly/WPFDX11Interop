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

// Note from Geometric Tools (01Jul2022). This file is a significant
// modification of the Microsoft sample file SurfaceQueueInteropHelper.h.
// 
//   1. The surface queue support was removed completely in order to
//      illustrate the core concepts for interoperability with Direct3D10/11.
//      The queue code was complicated and for some unknown reason was
//      written to use COM interfaces, making it difficult to step through
//      with a debugger to extract the core concepts. The current class,
//      DXManager, is simply that--a management layer for the core concepts
//      of interoperability with DirectX.
// 
//   2. Removed as much lazy initialization as possible. At program startup,
//      various interfaces were created and then immediately destroyed. Some
//      logic changes avoid this. The Render(bool resize) function is the
//      stripped down SurfaceQueueInteropHelper::QueueHelper function. The
//      Boolean input is 'true' when the window is being resized; it lets
//      the Render function know that the underlying Direct3D resources must
//      be recreated. The Boolean value is 'false' during a normal rendering
//      event.
// 
//   3. It was not clear why the namespace Microsoft:Windows:Media was used
//      to wrap the queue code. The namespace System.Windows.Interop.DirectX
//      is used instead because the already existing D3DImage class is in the
//      namespace System.Windows.Interop.
//
//   4. The member names were modified to be consistent with GTE conventions.

#pragma once

#include <d3d9.h>
#include <d3d10_1.h>

using namespace System;
using namespace System::Windows;
using namespace System::Windows::Interop;

namespace System {
    namespace Windows {
        namespace Interop {
            namespace DirectX {

                public ref class DXManager : IDisposable
                {
                private:
                    Action<IntPtr, bool>^ mOnRender;
                    D3DImage^ mD3DImage;
                    UINT mWidth, mHeight;
                    HWND mHWnd;
                    IDirect3D9Ex* mD3D9;
                    IDirect3DDevice9Ex* mD3D9Device;
                    ID3D10Device1* mD3D10Device;
                    IDirect3DSurface9* mD3D9Surface;
                    IDXGISurface* mDXGISurface;
                    bool mInitialized;

                public:
                    DXManager();
                    !DXManager();
                    ~DXManager();

                    property Action<IntPtr, bool>^ DXManager::OnRender
                    {
                        void set(Action<IntPtr, bool>^ value) { mOnRender = value; }
                    }

                    property D3DImage^ DXManager::D3DImage
                    {
                        System::Windows::Interop::D3DImage^ get()
                        {
                            return mD3DImage;
                        }

                        void set(System::Windows::Interop::D3DImage^ d3dImage)
                        {
                            if (d3dImage != mD3DImage)
                            {
                                if (nullptr != mD3DImage)
                                {
                                    mD3DImage->SetBackBuffer(
                                        System::Windows::Interop::D3DResourceType::IDirect3DSurface9,
                                        (IntPtr)nullptr);
                                }

                                mD3DImage = d3dImage;
                            }
                        }
                    }

                    property unsigned int DXManager::Width
                    {
                        unsigned int get()
                        {
                            return mWidth;
                        }
                    }

                    property unsigned int DXManager::Height
                    {
                        unsigned int get()
                        {
                            return mHeight;
                        }
                    }

                    property IntPtr DXManager::HWND
                    {
                        IntPtr get() { return (IntPtr)(void*)mHWnd; }
                        void set(IntPtr hwnd) { mHWnd = (::HWND)(void*)hwnd; }
                    }

                    void OnResize(unsigned int width, unsigned int height);
                    void OnRequestRender();

                private:
                    bool Initialize();
                    bool InitializeD3D9Ex();
                    bool InitializeD3D10();
                    void Terminate();
                    bool CreateSharedSurface();
                    void Render(bool resize);
                };

            }
        }
    }
}
