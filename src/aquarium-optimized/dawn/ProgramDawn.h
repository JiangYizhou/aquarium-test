//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// ProgramDawn.h: Defines Program wrapper of Dawn.
// Load shaders from folder shaders/dawn.

#pragma once
#ifndef PROGRAMGL_H
#define PROGRAMGL_H 1

#include "../Program.h"

#ifndef SHADERLOADER_H
#define SHADERLOADER_H

#include <string>
#include <unordered_map>

#include "BufferDawn.h"
#include "TextureDawn.h"

#include "../Aquarium.h"

class ContextDawn;

class ProgramDawn : public Program
{
public:
    ProgramDawn() {}
    ProgramDawn(ContextDawn *context, const std::string &vId, const std::string &fId);
    ~ProgramDawn() override;

    void loadProgram();
    dawn::ShaderModule getVSModule() { return vsModule; }
    dawn::ShaderModule getFSModule() { return fsModule; }

private:

    dawn::ShaderModule vsModule;
    dawn::ShaderModule fsModule;

    ContextDawn *context;
};

#endif

#endif // !PROGRAMGL_H
