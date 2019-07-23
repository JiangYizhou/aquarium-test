//
// Copyright (c) 2019 The Native Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Model.cpp: Implement common functions of Model.

#include "Aquarium.h"
#include "Buffer.h"
#include "Model.h"

Model::Model()
    : mProgram(nullptr),
      mBlend(false),
      mName(MODELMAX)
{
}

Model::~Model()
{
    for (auto &buf : bufferMap)
    {
        if (buf.second != nullptr)
        {
            delete buf.second;
            buf.second = nullptr;
        }
    }
}

void Model::setProgram(Program *prgm)
{
    mProgram = prgm;
}
