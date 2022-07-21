// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2022
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
// https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// Version: 1.0.2022.07.01

#pragma once

#include "../DX11Native/Application.h"
#include <cstdint>
using namespace System;

namespace System {
    namespace Windows {
        namespace Interop {
            namespace DirectX {

                public ref class DX11Managed
                {
                public:
                    // If any of the member functions fails because the native
                    // code threw an exception, the description of the exception
                    // is contained by exceptionMessage. On success, the string
                    // is "".
                    DX11Managed();
                    ~DX11Managed();
                    !DX11Managed();

                    // If RenderFrame succeeds, the return value is 'true' and
                    // the exceptionMessage is "". If RenderFrame fails, the return
                    // value is 'false' and the exceptionMessage contains a
                    // description of the exception thrown by the native code.
                    bool RenderFrame(
                        IntPtr wpfBackBuffer,
                        bool recreateRenderTarget);

                    String^ exceptionMessage;

                private:
                    dxm::Application* mInstance;
                    bool exceptionMessageUnassigned;
                };

            }
        }
    }
}
