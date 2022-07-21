// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2022
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
// https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// Version: 1.0.2022.07.01

#include "Application.h"
#include <stdexcept>
using namespace dxm;

Application* Application::Create(std::string& exceptionMessage)
{
    Application* application = nullptr;

    try
    {
        application = new Application();
        exceptionMessage = "";
    }
    catch (std::exception& e)
    {
        exceptionMessage = e.what();
    }

    return application;
}

std::string Application::Destroy(Application* application)
{
    std::string exceptionMessage = "";

    if (application)
    {
        try
        {
            delete application;
        }
        catch (std::exception& e)
        {
            exceptionMessage = e.what();
        }
    }
    else
    {
        exceptionMessage = "Expecting null pointer to Application::Delete";
    }

    return exceptionMessage;
}

std::string Application::RenderFrame(Application* application,
    void* wpfBackBuffer, bool recreateRenderTarget)
{
    std::string exceptionMessage = "";

    if (application)
    {
        try
        {
            application->RenderFrame(wpfBackBuffer, recreateRenderTarget);
        }
        catch (std::exception& e)
        {
            exceptionMessage = e.what();
        }
    }
    else
    {
        exceptionMessage = "Expecting null pointer to Application::RenderFrame";
    }

    return exceptionMessage;
}

Application::Application()
    :
    mDevice(nullptr),
    mContext(nullptr),
    mXSize(0),
    mYSize(0),
    mRenderTargetView(nullptr),
    mDRE{},
    mURD(0.0f, 1.0f),
    mClearColor{ 0.0f, 0.0f, 1.0f, 1.0f }
{
    // To enable the DirectX Debug Layer, OR in D3D11_CREATE_DEVICE_DEBUG.
    // You then need to run the DirectX Control Panel and add the executable
    // to its list of programs to monitor.
    std::array<D3D_FEATURE_LEVEL, 4> const featureLevels =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };

    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_1_0_CORE;
    bool success = false;
    for (size_t i = 0; i < featureLevels.size(); ++i)
    {
        HRESULT hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            flags,
            &featureLevels[i],
            1, D3D11_SDK_VERSION,
            &mDevice,
            &featureLevel,
            &mContext);

        if (SUCCEEDED(hr))
        {
            success = true;

            if (featureLevel == D3D_FEATURE_LEVEL_11_1 ||
                featureLevel == D3D_FEATURE_LEVEL_11_0)
            {
                // shader model 5.0.
                break;
            }

            if (featureLevel == D3D_FEATURE_LEVEL_10_1)
            {
                // shader model 4.1
                break;
            }

            if (featureLevel == D3D_FEATURE_LEVEL_10_0)
            {
                // shader model 4.0
                break;
            }
        }
    }

    if (!success)
    {
        throw std::runtime_error("Failed to create device.");
    }
}

Application::~Application()
{
    ReleaseInterface(mRenderTargetView);
    ReleaseInterface(mContext);
    ULONG refs = ReleaseInterface(mDevice);
    (void)refs;  // This should be 0.
}

void Application::RenderFrame(void* wpfBackBuffer, bool recreateRenderTarget)
{
    if (recreateRenderTarget || mRenderTargetView == nullptr)
    {
        RecreateRenderTarget(wpfBackBuffer);
        for (size_t i = 0; i < 3; ++i)
        {
            mClearColor[i] = mURD(mDRE);
        }
    }

    mContext->OMSetRenderTargets(1, &mRenderTargetView, nullptr);
    mContext->ClearRenderTargetView(mRenderTargetView, mClearColor.data());
    {
        // DO YOUR RENDERING HERE
    }
    mContext->OMSetRenderTargets(0, nullptr, nullptr);

    // The online posts indicate that mContext->Flush() should be called.
    // However, WPF renders in a thread different from the one for this
    // DX11 code. Flush() is not blocking, so returns immediately while
    // the GPU does its work asynchronously. On return from RenderFrame,
    // the lock is released on the D3DImage object, so WPF draws to the
    // shared back buffer. But if the GPU is not yet finished, this leads
    // to concurrent writing by two threads. Instead, you have to wait
    // for the GPU to finish to be sure that WPF can draw safely.
    D3D11_QUERY_DESC desc{};
    desc.Query = D3D11_QUERY_EVENT;
    desc.MiscFlags = 0u;
    ID3D11Query* waitQuery = nullptr;
    HRESULT hr = mDevice->CreateQuery(&desc, &waitQuery);
    if (FAILED(hr))
    {
        throw std::runtime_error("CreateQuery failed.");
    }

    mContext->End(waitQuery);
    BOOL data = 0;
    UINT size = sizeof(BOOL);
    while (S_OK != mContext->GetData(waitQuery, &data, size, 0))
    {
        // Wait for the GPU to finish computing its current commands.
    }
    ReleaseInterface(waitQuery);
}

void Application::RecreateRenderTarget(void* wpfBackBuffer)
{
    mContext->OMSetRenderTargets(0, nullptr, nullptr);

    IUnknown* unknown = reinterpret_cast<IUnknown*>(wpfBackBuffer);
    IDXGIResource* dxgiResource = nullptr;
    HRESULT hr = unknown->QueryInterface(__uuidof(IDXGIResource),
        (void**)&dxgiResource);
    if (FAILED(hr))
    {
        throw std::runtime_error("dxgiResource QueryInterface failed");
    }

    HANDLE sharedHandle = nullptr;
    hr = dxgiResource->GetSharedHandle(&sharedHandle);
    if (FAILED(hr))
    {
        throw std::runtime_error("GetSharedHandle failed");
    }
    ReleaseInterface(dxgiResource);

    IUnknown* sharedResource = nullptr;
    hr = mDevice->OpenSharedResource(sharedHandle,
        __uuidof(ID3D11Resource), (void**)(&sharedResource));
    if (FAILED(hr))
    {
        throw std::runtime_error("OpenSharedResource failed");
    }

    ID3D11Texture2D* texture = nullptr;
    hr = sharedResource->QueryInterface(__uuidof(ID3D11Texture2D),
        (void**)(&texture));
    if (FAILED(hr))
    {
        throw std::runtime_error("sharedResource QueryInterface failed");
    }
    ReleaseInterface(sharedResource);

    D3D11_RENDER_TARGET_VIEW_DESC rtDesc{};
    rtDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtDesc.Texture2D.MipSlice = 0;

    ReleaseInterface(mRenderTargetView);
    hr = mDevice->CreateRenderTargetView(texture, &rtDesc, &mRenderTargetView);
    if (FAILED(hr))
    {
        throw std::runtime_error("CreateRenderTargetView failed");
    }

    D3D11_TEXTURE2D_DESC desc{};
    texture->GetDesc(&desc);
    mXSize = desc.Width;
    mYSize = desc.Height;
    ReleaseInterface(texture);

    D3D11_VIEWPORT viewport{};
    viewport.Width = static_cast<float>(mXSize);
    viewport.Height = static_cast<float>(mYSize);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    mContext->RSSetViewports(1, &viewport);
}
