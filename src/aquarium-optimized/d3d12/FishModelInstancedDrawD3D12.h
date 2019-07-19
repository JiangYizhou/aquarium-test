//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelD3D12InstancedDraw.h: Defnes fish model of D3D12

#pragma once
#ifndef FISHMODELD3D12INSTANCEDDRAW_H
#define FISHMODELD3D12INSTANCEDDRAW_H 1

#include <string>

#include "BufferD3D12.h"
#include "ContextD3D12.h"
#include "ProgramD3D12.h"
#include "TextureD3D12.h"

#include "../FishModel.h"

class FishModelInstancedDrawD3D12 : public FishModel
{
  public:
    FishModelInstancedDrawD3D12(Context *context,
                                Aquarium *aquarium,
                                MODELGROUP type,
                                MODELNAME name,
                                bool blend);
    ~FishModelInstancedDrawD3D12();

    void init() override;
    void prepareForDraw() const override;
    void draw() override;

    void updatePerInstanceUniforms(WorldUniforms *worldUniforms) override;
    void updateFishPerUniforms(float x,
                               float y,
                               float z,
                               float nextX,
                               float nextY,
                               float nextZ,
                               float scale,
                               float time,
                               int index) override;

    struct FishVertexUniforms
    {
        float fishLength;
        float fishWaveLength;
        float fishBendAmount;
    } fishVertexUniforms;

    struct LightFactorUniforms
    {
        float shininess;
        float specularFactor;
    } lightFactorUniforms;

    struct FishPer
    {
        float worldPosition[3];
        float scale;
        float nextPosition[3];
        float time;
    };
    FishPer *fishPers;

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

  private:
    D3D12_VERTEX_BUFFER_VIEW fishPersBufferView;
    ComPtr<ID3D12Resource> fishPersBuffer;

    D3D12_CONSTANT_BUFFER_VIEW_DESC lightFactorView;
    D3D12_GPU_DESCRIPTOR_HANDLE lightFactorGPUHandle;
    ComPtr<ID3D12Resource> lightFactorBuffer;
    ComPtr<ID3D12Resource> lightFactorUploadBuffer;

    D3D12_CONSTANT_BUFFER_VIEW_DESC fishVertexView;
    D3D12_GPU_DESCRIPTOR_HANDLE fishVertexGPUHandle;
    ComPtr<ID3D12Resource> fishVertexBuffer;
    ComPtr<ID3D12Resource> fishVertexUploadBuffer;

    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView[6];

    ComPtr<ID3D12RootSignature> m_rootSignature;

    ComPtr<ID3D12PipelineState> m_pipelineState;

    int instance;

    ProgramD3D12 *programD3D12;
    ContextD3D12 *contextD3D12;
};

#endif