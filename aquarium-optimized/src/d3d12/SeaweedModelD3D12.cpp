//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// SeaweenModelD3D12: Implements seaweed model of D3D12.

#include "SeaweedModelD3D12.h"

SeaweedModelD3D12::SeaweedModelD3D12(Context *context,
                                     Aquarium *aquarium,
                                     MODELGROUP type,
                                     MODELNAME name,
                                     bool blend)
    : SeaweedModel(type, name, blend), instance(0)
{
    contextD3D12 = static_cast<ContextD3D12 *>(context);
    mAquarium    = aquarium;

    lightFactorUniforms.shininess      = 50.0f;
    lightFactorUniforms.specularFactor = 1.0f;
}

void SeaweedModelD3D12::init()
{
    programD3D12 = static_cast<ProgramD3D12 *>(mProgram);

    diffuseTexture    = static_cast<TextureD3D12 *>(textureMap["diffuse"]);
    normalTexture     = static_cast<TextureD3D12 *>(textureMap["normalMap"]);
    reflectionTexture = static_cast<TextureD3D12 *>(textureMap["reflectionMap"]);
    skyboxTexture     = static_cast<TextureD3D12 *>(textureMap["skybox"]);

    positionBuffer = static_cast<BufferD3D12 *>(bufferMap["position"]);
    normalBuffer   = static_cast<BufferD3D12 *>(bufferMap["normal"]);
    texCoordBuffer = static_cast<BufferD3D12 *>(bufferMap["texCoord"]);
    indicesBuffer  = static_cast<BufferD3D12 *>(bufferMap["indices"]);

    vertexBufferView[0] = positionBuffer->mVertexBufferView;
    vertexBufferView[1] = normalBuffer->mVertexBufferView;
    vertexBufferView[2] = texCoordBuffer->mVertexBufferView;

    // create input layout
    inputElementDescs = {
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
         0},
    };

    // create constant buffer, desc.
    lightFactorBuffer = contextD3D12->createDefaultBuffer(
        &lightFactorUniforms, contextD3D12->CalcConstantBufferByteSize(sizeof(LightFactorUniforms)),
        lightFactorUploadBuffer);
    lightFactorView.BufferLocation = lightFactorBuffer->GetGPUVirtualAddress();
    lightFactorView.SizeInBytes    = contextD3D12->CalcConstantBufferByteSize(
        sizeof(LightFactorUniforms));  // CB size is required to be 256-byte aligned.
    contextD3D12->buildCbvDescriptor(lightFactorView, &lightFactorGPUHandle);
    worldBuffer = contextD3D12->createUploadBuffer(
        &worldUniformPer, contextD3D12->CalcConstantBufferByteSize(sizeof(WorldUniformPer)));
    worldBufferView.BufferLocation = worldBuffer->GetGPUVirtualAddress();
    worldBufferView.SizeInBytes = contextD3D12->CalcConstantBufferByteSize(sizeof(WorldUniformPer));
    seaweedBuffer               = contextD3D12->createUploadBuffer(
        &seaweedPer, contextD3D12->CalcConstantBufferByteSize(sizeof(SeaweedPer)));
    seaweedBufferView.BufferLocation = seaweedBuffer->GetGPUVirtualAddress();
    seaweedBufferView.SizeInBytes    = contextD3D12->CalcConstantBufferByteSize(sizeof(SeaweedPer));

    // Create root signature to bind resources.
    // Bind textures, samplers and immutable constant buffers in a descriptor table.
    // Bind frequently updated constant buffers by root descriptors.
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    CD3DX12_ROOT_PARAMETER1 rootParameters[6];
    CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
    rootParameters[0] = contextD3D12->rootParameterGeneral;
    rootParameters[1] = contextD3D12->rootParameterWorld;

    diffuseTexture->createSrvDescriptor();

    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 2,
                   D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 2,
                   D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    rootParameters[2].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[3].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);

    rootParameters[4].InitAsConstantBufferView(0, 3, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE,
                                               D3D12_SHADER_VISIBILITY_VERTEX);
    rootParameters[5].InitAsConstantBufferView(1, 3, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE,
                                               D3D12_SHADER_VISIBILITY_VERTEX);

    rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 2u,
                               contextD3D12->staticSamplers.data(),
                               D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    contextD3D12->createRootSignature(&rootSignatureDesc, m_rootSignature);

    contextD3D12->createGraphicsPipelineState(inputElementDescs, m_rootSignature,
                                              programD3D12->getVSModule(),
                                              programD3D12->getFSModule(), m_pipelineState, mBlend);
}

void SeaweedModelD3D12::preDraw() const
{
    CD3DX12_RANGE readRangeView(0, 0);
    UINT8 *m_pCbvDataBeginView;
    worldBuffer->Map(0, &readRangeView, reinterpret_cast<void **>(&m_pCbvDataBeginView));
    memcpy(m_pCbvDataBeginView, &worldUniformPer, sizeof(WorldUniformPer));

    CD3DX12_RANGE readRangeSeaweed(0, 0);
    UINT8 *m_pCbvDataBeginSeaweed;
    seaweedBuffer->Map(0, &readRangeSeaweed, reinterpret_cast<void **>(&m_pCbvDataBeginSeaweed));
    memcpy(m_pCbvDataBeginSeaweed, &seaweedPer, sizeof(SeaweedPer));
}

void SeaweedModelD3D12::draw()
{
    auto &commandList = contextD3D12->mCommandList;

    commandList->SetPipelineState(m_pipelineState.Get());
    commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    commandList->SetGraphicsRootDescriptorTable(0, contextD3D12->lightGPUHandle);
    commandList->SetGraphicsRootConstantBufferView(
        1, contextD3D12->lightWorldPositionView.BufferLocation);
    commandList->SetGraphicsRootDescriptorTable(2, lightFactorGPUHandle);
    commandList->SetGraphicsRootDescriptorTable(3, diffuseTexture->getTextureGPUHandle());
    commandList->SetGraphicsRootConstantBufferView(4, worldBufferView.BufferLocation);
    commandList->SetGraphicsRootConstantBufferView(5, seaweedBufferView.BufferLocation);

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, 3, vertexBufferView);

    commandList->IASetIndexBuffer(&indicesBuffer->mIndexBufferView);

    commandList->DrawIndexedInstanced(indicesBuffer->getTotalComponents(), instance, 0, 0, 0);

    instance = 0;
}

void SeaweedModelD3D12::updatePerInstanceUniforms(WorldUniforms *worldUniforms)
{
    worldUniformPer.worldUniforms[instance] = *worldUniforms;
    seaweedPer.seaweed[instance].time       = mAquarium->g.mclock + instance;

    instance++;
}

void SeaweedModelD3D12::updateSeaweedModelTime(float time) {}
