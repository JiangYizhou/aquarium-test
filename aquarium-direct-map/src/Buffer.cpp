//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Buffer.cpp: Implements the index or vertex buffer wrappers and resource bindings of OpenGL.

#include "Buffer.h"

#include <iostream>

#include "ASSERT.h"

Buffer::Buffer(AttribBuffer *attribBuffer, GLenum target)
    : buf(0),
    target(target),
    numComponents_(attribBuffer->getNumComponents()),
    numElements_(attribBuffer->getNumElements()),
    totalComponents_(0),
    type_(0),
    normalize_(true),
    stride_(0),
    offset_(nullptr)

{
    glGenBuffers(1, &buf);

    glBindBuffer(target, buf);

    totalComponents_ = numComponents_ * numElements_;

    auto bufferFloat  = attribBuffer->getBufferFloat();
    auto bufferUShort = attribBuffer->getBufferUShort();

    if (attribBuffer->getType() == "Float32Array")
    {
        type_      = GL_FLOAT;
        normalize_ = false;
        glBufferData(target, sizeof(GLfloat) * bufferFloat->size(), bufferFloat->data(),
                     GL_STATIC_DRAW);
    }
    else if (attribBuffer->getType() == "Uint16Array")
    {
        type_ = GL_UNSIGNED_SHORT;
        glBufferData(target, sizeof(GLushort) * bufferUShort->size(), bufferUShort->data(),
                     GL_STATIC_DRAW);
    }
    else
    {
        std::cout << "bindBufferData undefined type." << std::endl;
    }

    ASSERT(glGetError() == GL_NO_ERROR);
}

Buffer::~Buffer()
{
    glDeleteBuffers(1, &buf);
}
