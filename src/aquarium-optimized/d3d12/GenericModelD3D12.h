//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// GenericModelD3D12.h: Defnes generic model of D3D12

#pragma once
#ifndef GENERICMODELD3D12_H
#define GENERICMODELD3D12_H 1

#include <string>
#include <vector>

#include "BufferD3D12.h"
#include "ContextD3D12.h"
#include "ProgramD3D12.h"
#include "TextureD3D12.h"

#include "../Model.h"

class GenericModelD3D12 : public Model
{
  public:
    GenericModelD3D12(Context *context,
                      Aquarium *aquarium,
                      MODELGROUP type,
                      MODELNAME name,
                      bool blend);

    void init() override;
    void prepareForDraw() const override;
    void draw() override;

    void updatePerInstanceUniforms(WorldUniforms *worldUniforms) override;

    TextureD3D12 *diffuseTexture;
    TextureD3D12 *normalTexture;
    TextureD3D12 *reflectionTexture;
    TextureD3D12 *skyboxTexture;

    BufferD3D12 *positionBuffer;
    BufferD3D12 *normalBuffer;
    BufferD3D12 *texCoordBuffer;
    BufferD3D12 *tangentBuffer;
    BufferD3D12 *binormalBuffer;

    BufferD3D12 *indicesBuffer;

    struct LightFactorUniforms
    {
        float shininess;
        float specularFactor;
    } lightFactorUniforms;

    struct WorldUniformPer
    {
        WorldUniforms WorldUniforms[20];
    };
    WorldUniformPer worldUniformPer;

  private:
    D3D12_CONSTANT_BUFFER_VIEW_DESC worldBufferView;
    ComPtr<ID3D12Resource> worldBuffer;

    D3D12_CONSTANT_BUFFER_VIEW_DESC lightFactorView;
    D3D12_GPU_DESCRIPTOR_HANDLE lightFactorGPUHandle;
    ComPtr<ID3D12Resource> lightFactorBuffer;
    ComPtr<ID3D12Resource> lightFactorUploadBuffer;

    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView[5];

    ComPtr<ID3D12RootSignature> m_rootSignature;

    ComPtr<ID3D12PipelineState> m_pipelineState;

    ContextD3D12 *contextD3D12;
    ProgramD3D12 *programD3D12;

    int instance;
};

#endif
