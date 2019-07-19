//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// ProgramD3D.h: Defines Program wrapper of D3D.
// Load shaders from folder shaders/D3D.

#pragma once
#ifndef PROGRAMD3D_H
#define PROGRAMD3D_H 1


#ifndef SHADERLOADER_H
#define SHADERLOADER_H

#include <string>
#include <unordered_map>

#include "stdafx.h"

#include "../Aquarium.h"
#include "../Program.h"

using Microsoft::WRL::ComPtr;

class ContextD3D12;

class ProgramD3D12 : public Program
{
  public:
    ProgramD3D12() {}
    ProgramD3D12(ContextD3D12 *context, const std::string &mVId, const std::string &mFId);
    ~ProgramD3D12() override;

    void loadProgram();
    ComPtr<ID3DBlob> getVSModule() { return mVertexShader; }
    ComPtr<ID3DBlob> getFSModule() { return mPixelShader; }

  private:
    ComPtr<ID3DBlob> mVertexShader;
    ComPtr<ID3DBlob> mPixelShader;

    ContextD3D12 *context;
};

#endif

#endif  // !PROGRAMGL_H
