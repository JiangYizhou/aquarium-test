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
    : mUsageBit(isIndex ? dawn::BufferUsageBit::Index : dawn::BufferUsageBit::Vertex),
      mTotoalComponents(totalCmoponents),
      mStride(0),
      mOffset(nullptr)
{
    mSize = numComponents * sizeof(float);
    mBuf = context->createBufferFromData(buffer.data(), sizeof(float) * static_cast<int>(buffer.size()), mUsageBit);
}

BufferDawn::BufferDawn(ContextDawn* context,
                       int totalCmoponents,
                       int numComponents,
                       const std::vector<unsigned short> &buffer,
                       bool isIndex)
    : mUsageBit(isIndex ? dawn::BufferUsageBit::Index : dawn::BufferUsageBit::Vertex),
      mTotoalComponents(totalCmoponents),
      mStride(0),
      mOffset(nullptr)
{
    mSize = numComponents * sizeof(unsigned short);
    mBuf = context->createBufferFromData(buffer.data(), sizeof(unsigned short) * static_cast<int>(buffer.size()), mUsageBit);
}

BufferDawn::~BufferDawn() {}
