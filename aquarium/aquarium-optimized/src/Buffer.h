//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Buffer.h: Define abstract Buffer Class and buffer type enum.

#pragma once
#ifndef BUFFER_H
#define BUFFER_H 1

#include <vector>

class Buffer
{
  public:
    Buffer() {}
    Buffer(int numComponents, int numElements, const std::vector<float> &buffer){};
    Buffer(int numComponents, int numElements, const std::vector<short> &buffer){};
    virtual ~Buffer(){};
};

#endif
