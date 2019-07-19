//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// BufferDawn.h: Defines the buffer wrapper of dawn, abstracting the vetex and index buffer binding.

#pragma once
#ifndef BUFFERDAWN_H
#define BUFFERDAWN_H 1

#include <vector>
#include <dawn/dawncpp.h>

#include "../Buffer.h"

class ContextDawn;

class BufferDawn : public Buffer
{
  public:
    BufferDawn(ContextDawn *context,
               int totalCmoponents,
               int numComponents,
               std::vector<float> *buffer,
               bool isIndex);
    BufferDawn(ContextDawn *context,
               int totalCmoponents,
               int numComponents,
               std::vector<unsigned short> *buffer,
               bool isIndex);
    ~BufferDawn() override;

    const dawn::Buffer &getBuffer() const { return mBuf; }
    int getTotalComponents() const { return mTotoalComponents; }

    uint32_t getStride() const { return mStride; }
    const void *getOffset() const { return mOffset; }
    dawn::BufferUsageBit getUsageBit() const { return mUsageBit; }
    int getDataSize() { return mSize; }

  private:
    dawn::Buffer mBuf;
    dawn::BufferUsageBit mUsageBit;
    int mTotoalComponents;
    uint32_t mStride;
    void *mOffset;
    int mSize;
};

#endif

