//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include <fstream>
#include <regex>
#include <string>

#include "ContextDawn.h"
#include "ProgramDawn.h"
#include "common/AQUARIUM_ASSERT.h"

ProgramDawn::ProgramDawn(ContextDawn *context, std::string vId, std::string fId)
    : Program(vId, fId), mVsModule(nullptr), mFsModule(nullptr), mContext(context)
{
}

ProgramDawn::~ProgramDawn()
{
    mVsModule = nullptr;
    mFsModule = nullptr;
}

void ProgramDawn::loadProgram()
{
    std::ifstream VertexShaderStream(mVId, std::ios::in);
    std::string VertexShaderCode((std::istreambuf_iterator<char>(VertexShaderStream)),
        std::istreambuf_iterator<char>());
    VertexShaderStream.close();

    // Read the Fragment Shader code from the file
    std::ifstream FragmentShaderStream(mFId, std::ios::in);
    std::string FragmentShaderCode((std::istreambuf_iterator<char>(FragmentShaderStream)),
        std::istreambuf_iterator<char>());
    FragmentShaderStream.close();

    FragmentShaderCode =
        std::regex_replace(FragmentShaderCode, std::regex(R"(\n.*?// #noReflection)"), "");
    FragmentShaderCode =
        std::regex_replace(FragmentShaderCode, std::regex(R"(\n.*?// #noNormalMap)"), "");

    mVsModule = mContext->createShaderModule(utils::ShaderStage::Vertex, VertexShaderCode);
    mFsModule = mContext->createShaderModule(utils::ShaderStage::Fragment, FragmentShaderCode);
}
