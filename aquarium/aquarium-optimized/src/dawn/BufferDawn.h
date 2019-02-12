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

#include "../Buffer.h"
#include <dawn/dawncpp.h>

class ContextDawn;

class BufferDawn : public Buffer
{
  public:
    BufferDawn(ContextDawn *context,
               int totalCmoponents,
               int numComponents,
               const std::vector<float> &buffer,
               bool isIndex);
    BufferDawn(ContextDawn *context,
               int totalCmoponents,
               int numComponents,
               const std::vector<unsigned short> &buffer,
               bool isIndex);
    ~BufferDawn() override;

    const dawn::Buffer &getBuffer() const { return buf; }
    const int getNumComponents() const { return numComponents_; }
    const int getTotalComponents() const { return totoalComponents_; }
    //const int getNumberElements() const { return numElements_; }
    const bool getNormalize() const { return normalize_; }
    const uint32_t getStride() const { return stride_; }
    const void *getOffset() const { return offset_; }
    const dawn::BufferUsageBit getUsageBit() const { return usageBit; }
    int getDataSize() { return size; }

  private:
    dawn::Buffer buf;
    dawn::BufferUsageBit usageBit;
    int numComponents_;
    int totoalComponents_;
    //int numElements_;
    bool normalize_;
    uint32_t stride_;
    void *offset_;
    int size;
};

#endif

