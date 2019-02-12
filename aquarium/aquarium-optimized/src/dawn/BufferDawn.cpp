//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// BufferDawn.cpp: Implements the index or vertex buffers wrappers and resource bindings of dawn.

#include "BufferDawn.h"

#include "ContextDawn.h"

BufferDawn::BufferDawn(ContextDawn* context,
                       int totalCmoponents,
                       int numComponents,
                       const std::vector<float> &buffer,
                       bool isIndex)
    : usageBit(isIndex ? dawn::BufferUsageBit::Index : dawn::BufferUsageBit::Vertex),
      totoalComponents_(totalCmoponents),
      stride_(0),
      offset_(nullptr)
{
    //numElements_ = totoalComponents_ / numComponents;
    size = numComponents * sizeof(float);
    buf = context->createBufferFromData(buffer.data(), sizeof(float) * buffer.size(), usageBit);
}

BufferDawn::BufferDawn(ContextDawn* context,
                       int totalCmoponents,
                       int numComponents,
                       const std::vector<unsigned short> &buffer,
                       bool isIndex)
    : usageBit(isIndex ? dawn::BufferUsageBit::Index : dawn::BufferUsageBit::Vertex),
      totoalComponents_(totalCmoponents),
      stride_(0),
      offset_(nullptr)
{
    //numElements_ = totoalComponents_ / numComponents;
    size = numComponents * sizeof(unsigned short);
    buf = context->createBufferFromData(buffer.data(), sizeof(unsigned short) * buffer.size(), usageBit);
}

BufferDawn::~BufferDawn() {}
