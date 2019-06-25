//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// InnerModelD3D12.h: Defines inner model of D3D12.

#pragma once
#ifndef INNERMODELD3D12_H
#define INNERMODELD3D12_H 1

#include "../Model.h"
#include "BufferD3D12.h"
#include "ContextD3D12.h"
#include "ProgramD3D12.h"
#include "TextureD3D12.h"

class InnerModelD3D12 : public Model
{
  public:
    InnerModelD3D12(Context *context,
                    Aquarium *aquarium,
                    MODELGROUP type,
                    MODELNAME name,
                    bool blend);

    void init() override;
    void preDraw() const override;
    void draw() override;
    void updatePerInstanceUniforms(WorldUniforms *WorldUniforms) override;

    struct InnerUniforms
    {
        float eta;
        float tankColorFudge;
        float refractionFudge;
        float padding;
    } innerUniforms;

    WorldUniforms worldUniformPer;

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
    D3D12_CONSTANT_BUFFER_VIEW_DESC worldBufferView;
    ComPtr<ID3D12Resource> worldBuffer;

    D3D12_CONSTANT_BUFFER_VIEW_DESC InnerView;
    D3D12_GPU_DESCRIPTOR_HANDLE InnerGPUHandle;
    ComPtr<ID3D12Resource> InnerBuffer;
    ComPtr<ID3D12Resource> InnerUploadBuffer;

    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView[5];

    ComPtr<ID3D12RootSignature> m_rootSignature;

    ComPtr<ID3D12PipelineState> m_pipelineState;

    ContextD3D12 *contextD3D12;
    ProgramD3D12 *programD3D12;
};

#endif  // !INNERMODELD3D12_H
#pragma once
