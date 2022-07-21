// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2022
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
// https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// Version: 1.0.2022.07.01
#pragma once

#include <d3d11.h>
#include <array>
#include <string>

// The random number generator is used to set the clear color during a
// resize, acting as a visual confirmation that the render target is
// recreated. It is not necessary for the general WPF-DX11 interop.
#include <random>

namespace dxm
{
    class Application
    {
    public:

        static Application* Create(std::string& exceptionMessage);

        static std::string Destroy(Application* application);

        static std::string RenderFrame(Application* application,
            void* wpfBackBuffer, bool recreateRenderTarget);

    private:
        Application();
        ~Application();

        void RenderFrame(void* wpfBackBuffer, bool recreateRenderTarget);

        void RecreateRenderTarget(void* wpfBackBuffer);

        template <typename T>
        ULONG ReleaseInterface(T*& object)
        {
            ULONG refs;
            if (object)
            {
                refs = object->Release();
                object = nullptr;
            }
            else
            {
                refs = 0;
            }
            return refs;
        }

        ID3D11Device* mDevice;
        ID3D11DeviceContext* mContext;
        uint32_t mXSize, mYSize;
        ID3D11RenderTargetView* mRenderTargetView;

        // See the comments before the #include <random>.
        std::default_random_engine mDRE;
        std::uniform_real_distribution<float> mURD;
        std::array<float, 4> mClearColor;
    };
}
