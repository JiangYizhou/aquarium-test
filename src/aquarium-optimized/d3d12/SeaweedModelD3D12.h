//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// SeaweedModelD3D12.h: Defines seaweed model of D3D12.

#pragma once
#ifndef SEAWEEDMODELD3D12_H
#define SEAWEEDMODELD3D12_H 1

#include "BufferD3D12.h"
#include "ContextD3D12.h"
#include "ProgramD3D12.h"
#include "TextureD3D12.h"

#include "../SeaweedModel.h"

class SeaweedModelD3D12 : public SeaweedModel
{
  public:
    SeaweedModelD3D12(Context *context,
                      Aquarium *aquarium,
                      MODELGROUP type,
                      MODELNAME name,
                      bool blend);

    void init() override;
    void prepareForDraw() const override;
    void draw() override;

    void updatePerInstanceUniforms(const WorldUniforms &worldUniforms) override;

    TextureD3D12 *diffuseTexture;
    TextureD3D12 *normalTexture;
    TextureD3D12 *reflectionTexture;
    TextureD3D12 *skyboxTexture;

    BufferD3D12 *positionBuffer;
    BufferD3D12 *normalBuffer;
    BufferD3D12 *texCoordBuffer;

    BufferD3D12 *indicesBuffer;
    void updateSeaweedModelTime(float time) override;

    struct LightFactorUniforms
    {
        float shininess;
        float specularFactor;
    } lightFactorUniforms;

    struct Seaweed
    {
        float time;
        float padding[3];
    };
    struct SeaweedPer
    {
        Seaweed seaweed[20];
    } seaweedPer;

    struct WorldUniformPer
    {
        WorldUniforms worldUniforms[20];
    };
    WorldUniformPer worldUniformPer;

  private:
    D3D12_CONSTANT_BUFFER_VIEW_DESC worldBufferView;
    ComPtr<ID3D12Resource> worldBuffer;
    D3D12_CONSTANT_BUFFER_VIEW_DESC seaweedBufferView;
    ComPtr<ID3D12Resource> seaweedBuffer;

    D3D12_CONSTANT_BUFFER_VIEW_DESC lightFactorView;
    D3D12_GPU_DESCRIPTOR_HANDLE lightFactorGPUHandle;
    ComPtr<ID3D12Resource> lightFactorBuffer;
    ComPtr<ID3D12Resource> lightFactorUploadBuffer;

    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView[3];

    ComPtr<ID3D12RootSignature> m_rootSignature;

    ComPtr<ID3D12PipelineState> m_pipelineState;

    

    ContextD3D12 *contextD3D12;
    ProgramD3D12 *programD3D12;
    Aquarium *mAquarium;

    int instance;
};

#endif  // !SEAWEEDMODEL_H