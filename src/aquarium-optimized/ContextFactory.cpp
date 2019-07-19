//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "Aquarium.h"
#include "ContextFactory.h"

#include "opengl/ContextGL.h"
#ifdef ENABLE_DAWN_BACKEND
#include "dawn/ContextDawn.h"
#endif
#ifdef ENABLE_D3D12_BACKEND
#include "d3d12/ContextD3D12.h"
#endif

ContextFactory::ContextFactory()
    :context(nullptr)
{}

ContextFactory::~ContextFactory()
{
    delete context;
}

Context *ContextFactory::createContext(BACKENDTYPE backendType)
{
    switch (backendType)
    {
        case BACKENDTYPE::BACKENDTYPEOPENGL:
        case BACKENDTYPE::BACKENDTYPEANGLE:
            {
                context = new ContextGL(backendType);
                break;
            }
        case BACKENDTYPE::BACKENDTYPEDAWND3D12:
        case BACKENDTYPE::BACKENDTYPEDAWNMETAL:
        case BACKENDTYPE::BACKENDTYPEDAWNVULKAN:
            {
#ifdef ENABLE_DAWN_BACKEND
                context = new ContextDawn(backendType);
#endif
                break;
            }
            case BACKENDTYPE::BACKENDTYPED3D12:
            {
#ifdef ENABLE_D3D12_BACKEND
                context = new ContextD3D12(backendType);
                break;
#endif
            }
            default:
                break;
    }

    return context;
}
