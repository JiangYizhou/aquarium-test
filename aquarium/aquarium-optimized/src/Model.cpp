//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Model.cpp: Implement common functions of Model.

#include "Model.h"

void Model::setProgram(Program *prgm)
{
    mProgram = prgm;
}

Model::Model()
    : mProgram(nullptr),
    mType(GROUPMAX),
    mName(MODELMAX),
    mBlend(false)
{
}

Model::~Model()
{
    for (auto buf : bufferMap)
    {
        if (buf.second != nullptr)
        {
            delete buf.second;
            buf.second = nullptr;
        }
    }
}

void Model::applyPrograms() const
{
    mProgram->setProgram();
}

void Model::prepare(const Context *context)
{
    context->enableBlend(mBlend);
    applyPrograms();
    applyBuffers();
    applyTextures();
    applyUniforms();
}
