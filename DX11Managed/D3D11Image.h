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

// Note from Geometric Tools (01Jul2022). This file has only minor
// modifications of the Microsoft sample file D3D11Image.h.
// 
//   1. The SurfaceQueueInteropHelper class is replaced by DXManager.
// 
//   2. The EnsureHelper function was removed.
// 
//   3. The function SetPixelSize was renamed to Resize and the signature
//      now uses unsigned integers rather than signed integes.
// 
//   4. The 'using Microsoft:Windows:Media;' statement was removed.

#pragma once

#include "DXManager.h"

using namespace System;
using namespace System::Windows;
using namespace System::Windows::Interop;

namespace System {
    namespace Windows {
        namespace Interop {
            namespace DirectX {

                public ref class D3D11Image :
                    public D3DImage
                {
                private:
                    static void RenderChanged(DependencyObject^ sender, DependencyPropertyChangedEventArgs args);
                    static void HWNDOwnerChanged(DependencyObject^ sender, DependencyPropertyChangedEventArgs args);
                    static D3D11Image();

                internal:
                    DXManager^ manager;

                protected:
                    Freezable^ CreateInstanceCore() override;

                public:
                    D3D11Image();
                    ~D3D11Image();

                    static DependencyProperty^ OnRenderProperty;
                    static DependencyProperty^ WindowOwnerProperty;

                    property Action<IntPtr, bool>^ OnRender
                    {
                        Action<IntPtr, bool>^ get()
                        {
                            return static_cast<Action<IntPtr, bool>^>(GetValue(OnRenderProperty));
                        }

                        void set(Action<IntPtr, bool>^ value)
                        {
                            SetValue(OnRenderProperty, value);
                        }
                    }

                    property IntPtr WindowOwner
                    {
                        IntPtr get()
                        {
                            return static_cast<IntPtr>(GetValue(WindowOwnerProperty));
                        }

                        void set(IntPtr value)
                        {
                            SetValue(WindowOwnerProperty, value);
                        }
                    }

                    void RequestRender();
                    void Resize(unsigned int width, unsigned int height);
                };
            }
        }
    }
}