//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#include "ContextFactory.h"
#include "opengl/ContextGL.h"
#ifndef EGL_EGL_PROTOTYPES
#include "dawn/ContextDawn.h"
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
       context = new ContextDawn();
    }

    return context;
}
