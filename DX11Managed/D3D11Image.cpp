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

#include "D3D11Image.h"

namespace System {
    namespace Windows {
        namespace Interop {
            namespace DirectX {

                static D3D11Image::D3D11Image()
                {
                    OnRenderProperty = DependencyProperty::Register("OnRender",
                        Action<IntPtr, bool>::typeid,
                        D3D11Image::typeid,
                        gcnew UIPropertyMetadata(nullptr, gcnew PropertyChangedCallback(&RenderChanged)));

                    WindowOwnerProperty = DependencyProperty::Register("WindowOwner",
                        IntPtr::typeid,
                        D3D11Image::typeid,
                        gcnew UIPropertyMetadata(IntPtr::Zero, gcnew PropertyChangedCallback(&HWNDOwnerChanged)));
                }

                D3D11Image::D3D11Image()
                {
                }

                D3D11Image::~D3D11Image()
                {
                    if (this->manager != nullptr)
                    {
                        this->manager->~DXManager();
                        this->manager = nullptr;
                    }
                }

                Freezable^ D3D11Image::CreateInstanceCore()
                {
                    return gcnew D3D11Image();
                }

                void D3D11Image::HWNDOwnerChanged(DependencyObject^ sender, DependencyPropertyChangedEventArgs args)
                {
                    D3D11Image^ image = dynamic_cast<D3D11Image^>(sender);

                    if (image != nullptr)
                    {
                        if (image->manager != nullptr)
                        {
                            image->manager->HWND = static_cast<IntPtr>(args.NewValue);
                        }
                    }
                }

                void D3D11Image::RenderChanged(DependencyObject^ sender, DependencyPropertyChangedEventArgs args)
                {
                    D3D11Image^ image = dynamic_cast<D3D11Image^>(sender);

                    if (image != nullptr)
                    {
                        if (image->manager != nullptr)
                        {
                            image->manager->OnRender = static_cast<Action<IntPtr, bool>^>(args.NewValue);
                        }
                    }
                }

                void D3D11Image::RequestRender()
                {
                    if (nullptr != this->OnRender)
                    {
                        if (this->manager == nullptr)
                        {
                            this->manager = gcnew DXManager();
                            this->manager->HWND = this->WindowOwner;
                            this->manager->D3DImage = this;
                            this->manager->OnRender = this->OnRender;
                        }

                        this->manager->OnRequestRender();
                    }
                }

                void D3D11Image::Resize(unsigned int width, unsigned int height)
                {
                    if (this->manager == nullptr)
                    {
                        this->manager = gcnew DXManager();
                        this->manager->HWND = this->WindowOwner;
                        this->manager->D3DImage = this;
                        this->manager->OnRender = this->OnRender;
                    }

                    this->manager->OnResize(width, height);
                }
            }
        }
    }
}
