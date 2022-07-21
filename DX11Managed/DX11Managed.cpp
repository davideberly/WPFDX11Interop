// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2022
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
// https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// Version: 1.0.2022.07.01

#include <msclr/marshal_cppstd.h>
#include "DX11Managed.h"
using namespace System::Runtime::InteropServices;

namespace System {
    namespace Windows {
        namespace Interop {
            namespace DirectX {

                DX11Managed::DX11Managed()
                    :
                    exceptionMessage(nullptr),
                    mInstance(nullptr),
                    exceptionMessageUnassigned(true)
                {
                    std::string nativeExceptionMessage;
                    mInstance = dxm::Application::Create(nativeExceptionMessage);
                    exceptionMessage = msclr::interop::marshal_as<String^>(nativeExceptionMessage);
                }

                DX11Managed::~DX11Managed()
                {
                    if (mInstance != nullptr)
                    {
                        std::string nativeExceptionMessage = dxm::Application::Destroy(mInstance);
                        exceptionMessage = msclr::interop::marshal_as<String^>(nativeExceptionMessage);
                        mInstance = nullptr;
                    }
                }

                DX11Managed::!DX11Managed()
                {
                    if (mInstance != nullptr)
                    {
                        std::string nativeExceptionMessage = dxm::Application::Destroy(mInstance);
                        exceptionMessage = msclr::interop::marshal_as<String^>(nativeExceptionMessage);
                        mInstance = nullptr;
                    }
                }

                bool DX11Managed::RenderFrame(
                    IntPtr wpfBackBuffer,
                    bool recreateRenderTarget)
                {
                    if (mInstance)
                    {
                        std::string nativeExceptionMessage = dxm::Application::RenderFrame(
                            mInstance, (void*)wpfBackBuffer, recreateRenderTarget);
                        exceptionMessage = msclr::interop::marshal_as<String^>(nativeExceptionMessage);
                        return (nativeExceptionMessage == "");
                    }
                    else
                    {
                        if (exceptionMessageUnassigned)
                        {
                            exceptionMessage = "Expecting an instance in RenderFrame.";
                            exceptionMessageUnassigned = false;
                        }
                        return false;
                    }
                }
            }
        }
    }
}
