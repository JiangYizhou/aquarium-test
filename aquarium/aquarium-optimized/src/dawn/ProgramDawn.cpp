//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#include "ProgramDawn.h"

#include "ContextDawn.h"
#include "../ASSERT.h"
#include <fstream>
#include <cstring>
#include <regex>

ProgramDawn::ProgramDawn(ContextDawn *context, std::string vId, std::string fId)
    : Program(vId, fId), vsModule(nullptr), fsModule(nullptr), context(context)
{
}

ProgramDawn::~ProgramDawn()
{
}

void ProgramDawn::loadProgram()
{
    std::ifstream VertexShaderStream(vId, std::ios::in);
    std::string VertexShaderCode((std::istreambuf_iterator<char>(VertexShaderStream)),
        std::istreambuf_iterator<char>());
    VertexShaderStream.close();

    // Read the Fragment Shader code from the file
    std::ifstream FragmentShaderStream(fId, std::ios::in);
    std::string FragmentShaderCode((std::istreambuf_iterator<char>(FragmentShaderStream)),
        std::istreambuf_iterator<char>());
    FragmentShaderStream.close();

#ifdef _WIN32
    FragmentShaderCode =
        std::regex_replace(FragmentShaderCode, std::regex(R"(^.*?// #noReflection\n)"), "");
    FragmentShaderCode =
        std::regex_replace(FragmentShaderCode, std::regex(R"(^.*?// #noNormalMap\n)"), "");
#else
    FragmentShaderCode =
        std::regex_replace(FragmentShaderCode, std::regex(R"(\n.*?// #noReflection)"), "");
    FragmentShaderCode =
        std::regex_replace(FragmentShaderCode, std::regex(R"(\n.*?// #noNormalMap)"), "");
#endif

    vsModule = context->createShaderModule(dawn::ShaderStage::Vertex, VertexShaderCode);
    fsModule = context->createShaderModule(dawn::ShaderStage::Fragment, FragmentShaderCode);
}
