//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
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

Context *ContextFactory::createContext(std::string str)
{
    if (str == "opengl" || str == "angle")
    {
        context = new ContextGL();
    }
    else if (str == "dawn")
    {
#ifdef ENABLE_DAWN_BACKEND
       context = new ContextDawn();
#endif
    }
    else if (str == "d3d12")
    {
#ifdef ENABLE_D3D12_BACKEND
        context = new ContextD3D12();
#endif
    }

    return context;
}
