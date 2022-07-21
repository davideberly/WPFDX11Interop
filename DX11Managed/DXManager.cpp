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
// modification of the Microsoft sample file SurfaceQueueInteropHelper.cpp.
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

#include "DXManager.h"

using namespace System;
using namespace System::Windows;
using namespace System::Windows::Interop;

#define ReleaseInterface(object) { if (object != nullptr) { object->Release(); object = nullptr; } }

namespace System {
    namespace Windows {
        namespace Interop {
            namespace DirectX {

                DXManager::DXManager()
                    :
                    mOnRender(nullptr),
                    mD3DImage(),
                    mWidth(0),
                    mHeight(0),
                    mHWnd(nullptr),
                    mD3D9(nullptr),
                    mD3D9Device(nullptr),
                    mD3D10Device(nullptr),
                    mD3D9Surface(nullptr),
                    mDXGISurface(nullptr),
                    mInitialized(false)
                {
                }

                DXManager::!DXManager()
                {
                    Terminate();
                }

                DXManager::~DXManager()
                {
                    Terminate();
                }

                void DXManager::OnResize(unsigned int width, unsigned int height)
                {
                    if (mWidth != width || mHeight != height)
                    {
                        mWidth = width;
                        mHeight = height;
                        if (mD3DImage != nullptr && mHWnd != nullptr)
                        {
                            Render(true);
                        }
                    }
                }

                void DXManager::OnRequestRender()
                {
                    if (mD3DImage != nullptr && mHWnd != nullptr)
                    {
                        Render(false);
                    }
                }

                bool DXManager::Initialize()
                {
                    if (!mInitialized)
                    {
                        if (InitializeD3D9Ex() && InitializeD3D10())
                        {
                            mInitialized = true;
                        }
                    }

                    return mInitialized;
                }

                bool DXManager::InitializeD3D9Ex()
                {
                    pin_ptr<IDirect3D9Ex*> pinD3D9Ex = &mD3D9;
                    HRESULT hr = Direct3DCreate9Ex(D3D_SDK_VERSION, pinD3D9Ex);
                    if (FAILED(hr))
                    {
                        return false;
                    }

                    D3DPRESENT_PARAMETERS presentParameters{};
                    ZeroMemory(&presentParameters, sizeof(presentParameters));
                    presentParameters.Windowed = TRUE;
                    presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
                    presentParameters.hDeviceWindow = NULL;
                    presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

                    DWORD behaviorFlags =
                        D3DCREATE_HARDWARE_VERTEXPROCESSING |
                        D3DCREATE_MULTITHREADED |
                        D3DCREATE_FPU_PRESERVE;

                    pin_ptr<IDirect3DDevice9Ex*> pinD3D9ExDevice = &mD3D9Device;
                    hr = mD3D9->CreateDeviceEx(
                        D3DADAPTER_DEFAULT,
                        D3DDEVTYPE_HAL,
                        mHWnd,
                        behaviorFlags,
                        &presentParameters,
                        NULL,
                        pinD3D9ExDevice);
                    if (FAILED(hr))
                    {
                        ReleaseInterface(mD3D9);
                        return false;
                    }

                    return true;
                }

                bool DXManager::InitializeD3D10()
                {
                    pin_ptr<ID3D10Device1*> pinD3D10Device = &mD3D10Device;
                    HRESULT hr = D3D10CreateDevice1(
                        nullptr,
                        D3D10_DRIVER_TYPE_HARDWARE,
                        nullptr,
                        D3D10_CREATE_DEVICE_BGRA_SUPPORT,
                        D3D10_FEATURE_LEVEL_10_0,
                        D3D10_1_SDK_VERSION,
                        pinD3D10Device);

                    return SUCCEEDED(hr);
                }

                void DXManager::Terminate()
                {
                    mInitialized = false;
                    ReleaseInterface(mDXGISurface);
                    ReleaseInterface(mD3D9Surface);
                    ReleaseInterface(mD3D10Device);
                    ReleaseInterface(mD3D9Device);
                    ReleaseInterface(mD3D9);
                }

                bool DXManager::CreateSharedSurface()
                {
                    IDirect3DTexture9* d3d9Texture = nullptr;
                    HANDLE sharedHandle = nullptr;
                    HRESULT hr = mD3D9Device->CreateTexture(mWidth, mHeight, 1,
                        D3DUSAGE_RENDERTARGET,
                        D3DFMT_A8R8G8B8,
                        D3DPOOL_DEFAULT,
                        (IDirect3DTexture9**)&d3d9Texture,
                        &sharedHandle);
                    if (FAILED(hr))
                    {
                        return false;
                    }

                    ReleaseInterface(mD3D9Surface);
                    pin_ptr<IDirect3DSurface9*> pinD3D9Surface = &mD3D9Surface;
                    hr = d3d9Texture->GetSurfaceLevel(0, pinD3D9Surface);
                    d3d9Texture->Release();
                    if (FAILED(hr))
                    {
                        ReleaseInterface(d3d9Texture);
                        return false;
                    }

                    ID3D10Texture2D* d3d10Texture = nullptr;
                    hr = mD3D10Device->OpenSharedResource(sharedHandle,
                        __uuidof(ID3D10Texture2D), (void**)&d3d10Texture);
                    if (FAILED(hr))
                    {
                        ReleaseInterface(mD3D9Surface);
                        ReleaseInterface(d3d9Texture);
                        return false;
                    }

                    ReleaseInterface(mDXGISurface);
                    pin_ptr<IDXGISurface*> pinDXGISurface = &mDXGISurface;
                    hr = d3d10Texture->QueryInterface(__uuidof(IDXGISurface),
                        (void**)pinDXGISurface);
                    d3d10Texture->Release();
                    if (FAILED(hr))
                    {
                        ReleaseInterface(d3d10Texture);
                        ReleaseInterface(mD3D9Surface);
                        ReleaseInterface(d3d9Texture);
                        return false;
                    }

                    return true;
                }

                void DXManager::Render(bool resize)
                {
                    if (!Initialize())
                    {
                        return;
                    }

                    if (mD3D9Surface == nullptr || resize)
                    {
                        if (!CreateSharedSurface())
                        {
                            return;
                        }
                    }

                    mD3DImage->Lock();
                    {
                        mOnRender((IntPtr)(void*)mDXGISurface, resize);

                        mD3DImage->SetBackBuffer(
                            System::Windows::Interop::D3DResourceType::IDirect3DSurface9,
                            (IntPtr)(void*)mD3D9Surface,
                            true);

                        mD3DImage->AddDirtyRect(
                            Int32Rect(0, 0, mD3DImage->PixelWidth, mD3DImage->PixelHeight));
                    }
                    mD3DImage->Unlock();
                }

            }
        }
    }
}
