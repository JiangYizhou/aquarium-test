//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// AttribBuffer.cpp: Implements AttribBuffer.

#include "AttribBuffer.h"

AttribBuffer::AttribBuffer(int numComponents,
                           const std::vector<float> &buffer,
                           int size,
                           std::string opt_type)
    : type(opt_type),
    bufferFloat(buffer),
    bufferUShort(NULL),
    numComponents(numComponents),
    numElements(size / numComponents)
{
}

AttribBuffer::AttribBuffer(int numComponents,
                           const std::vector<unsigned short> &buffer,
                           int size,
                           std::string opt_type)
    : type(opt_type),
    bufferFloat(NULL),
    bufferUShort(buffer),
    numComponents(numComponents),
    numElements(size / numComponents)
{
}
