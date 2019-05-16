//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// InnerModelD3D12.cpp: Implements inner model of D3D12.

#include "InnerModelD3D12.h"

InnerModelD3D12::InnerModelD3D12(Context *context,
                                 Aquarium *aquarium,
                                 MODELGROUP type,
                                 MODELNAME name,
                                 bool blend)
    : InnerModel(type, name, blend)
{
    contextD3D12 = static_cast<ContextD3D12 *>(context);

    innerUniforms.eta             = 1.0f;
    innerUniforms.tankColorFudge  = 0.796f;
    innerUniforms.refractionFudge = 3.0f;
}

void InnerModelD3D12::init()
{
    programD3D12 = static_cast<ProgramD3D12 *>(mProgram);

    diffuseTexture    = static_cast<TextureD3D12 *>(textureMap["diffuse"]);
    normalTexture     = static_cast<TextureD3D12 *>(textureMap["normalMap"]);
    reflectionTexture = static_cast<TextureD3D12 *>(textureMap["reflectionMap"]);
    skyboxTexture     = static_cast<TextureD3D12 *>(textureMap["skybox"]);

    positionBuffer = static_cast<BufferD3D12 *>(bufferMap["position"]);
    normalBuffer   = static_cast<BufferD3D12 *>(bufferMap["normal"]);
    texCoordBuffer = static_cast<BufferD3D12 *>(bufferMap["texCoord"]);
    tangentBuffer  = static_cast<BufferD3D12 *>(bufferMap["tangent"]);
    binormalBuffer = static_cast<BufferD3D12 *>(bufferMap["binormal"]);
    indicesBuffer  = static_cast<BufferD3D12 *>(bufferMap["indices"]);

    vertexBufferView[0] = positionBuffer->mVertexBufferView;
    vertexBufferView[1] = normalBuffer->mVertexBufferView;
    vertexBufferView[2] = texCoordBuffer->mVertexBufferView;
    vertexBufferView[3] = tangentBuffer->mVertexBufferView;
    vertexBufferView[4] = binormalBuffer->mVertexBufferView;

    inputElementDescs = {{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
                          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                         {"TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0,
                          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                         {"TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 2, 0,
                          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                         {"TEXCOORD", 3, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0,
                          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                         {"TEXCOORD", 4, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0,
                          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}};

    // create constant buffer, desc.
    InnerBuffer = contextD3D12->createDefaultBuffer(
        &innerUniforms, contextD3D12->CalcConstantBufferByteSize(sizeof(InnerUniforms)),
        InnerUploadBuffer);
    InnerView.BufferLocation = InnerBuffer->GetGPUVirtualAddress();
    InnerView.SizeInBytes    = contextD3D12->CalcConstantBufferByteSize(
        sizeof(InnerUniforms));  // CB size is required to be 256-byte aligned.
    contextD3D12->buildCbvDescriptor(InnerView, &InnerGPUHandle);
    worldBuffer = contextD3D12->createUploadBuffer(
        &worldUniformPer, contextD3D12->CalcConstantBufferByteSize(sizeof(WorldUniforms)));
    worldBufferView.BufferLocation = worldBuffer->GetGPUVirtualAddress();
    worldBufferView.SizeInBytes = contextD3D12->CalcConstantBufferByteSize(sizeof(WorldUniforms));

    // Create root signature to bind resources.
    // Bind textures, samplers and immutable constant buffers in a descriptor table.
    // Bind frequently updated constant buffers by root descriptors.
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    CD3DX12_ROOT_PARAMETER1 rootParameters[5];
    CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
    rootParameters[0] = contextD3D12->rootParameterGeneral;
    rootParameters[1] = contextD3D12->rootParameterWorld;

    diffuseTexture->createSrvDescriptor();
    normalTexture->createSrvDescriptor();
    reflectionTexture->createSrvDescriptor();

    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 2,
                   D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0, 2,
                   D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    rootParameters[2].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[3].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);

    rootParameters[4].InitAsConstantBufferView(0, 3, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE,
                                               D3D12_SHADER_VISIBILITY_VERTEX);

    rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 2u,
                               contextD3D12->staticSamplers.data(),
                               D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    contextD3D12->createRootSignature(&rootSignatureDesc, m_rootSignature);

    contextD3D12->createGraphicsPipelineState(inputElementDescs, m_rootSignature,
                                              programD3D12->getVSModule(),
                                              programD3D12->getFSModule(), m_pipelineState, mBlend);
}

void InnerModelD3D12::preDraw() const {}

void InnerModelD3D12::draw()
{
    auto &commandList = contextD3D12->mCommandList;

    commandList->SetPipelineState(m_pipelineState.Get());
    commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    commandList->SetGraphicsRootDescriptorTable(0, contextD3D12->lightGPUHandle);
    commandList->SetGraphicsRootConstantBufferView(
        1, contextD3D12->lightWorldPositionView.BufferLocation);
    commandList->SetGraphicsRootDescriptorTable(2, InnerGPUHandle);
    commandList->SetGraphicsRootDescriptorTable(3, diffuseTexture->getTextureGPUHandle());
    commandList->SetGraphicsRootConstantBufferView(4, worldBufferView.BufferLocation);

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, 5, vertexBufferView);

    commandList->IASetIndexBuffer(&indicesBuffer->mIndexBufferView);

    commandList->DrawIndexedInstanced(indicesBuffer->getTotalComponents(), 1, 0, 0, 0);
}

void InnerModelD3D12::updatePerInstanceUniforms(WorldUniforms *worldUniforms)
{
    memcpy(&worldUniformPer, worldUniforms, sizeof(WorldUniforms));

    CD3DX12_RANGE readRange(0, 0);
    UINT8 *m_pCbvDataBegin;
    worldBuffer->Map(0, &readRange, reinterpret_cast<void **>(&m_pCbvDataBegin));
    memcpy(m_pCbvDataBegin, &worldUniformPer, sizeof(WorldUniforms));
}
