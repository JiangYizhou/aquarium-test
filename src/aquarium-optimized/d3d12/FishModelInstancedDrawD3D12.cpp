//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelD3D12.cpp: Implements fish model of D3D12.

#include "BufferD3D12.h"
#include "FishModelInstancedDrawD3D12.h"

FishModelInstancedDrawD3D12::FishModelInstancedDrawD3D12(Context *context,
                                                         Aquarium *aquarium,
                                                         MODELGROUP type,
                                                         MODELNAME name,
                                                         bool blend)
    : FishModel(type, name, blend), instance(0)
{
    contextD3D12 = static_cast<ContextD3D12 *>(context);

    const Fish &fishInfo              = fishTable[name - MODELNAME::MODELSMALLFISHAINSTANCEDDRAWS];
    fishVertexUniforms.fishLength     = fishInfo.fishLength;
    fishVertexUniforms.fishBendAmount = fishInfo.fishBendAmount;
    fishVertexUniforms.fishWaveLength = fishInfo.fishWaveLength;

    lightFactorUniforms.shininess      = 5.0f;
    lightFactorUniforms.specularFactor = 0.3f;

    instance = aquarium->fishCount[fishInfo.modelName - MODELNAME::MODELSMALLFISHA];
    fishPers = new FishPer[instance];
}

FishModelInstancedDrawD3D12::~FishModelInstancedDrawD3D12()
{
    delete fishPers;
}

void FishModelInstancedDrawD3D12::init()
{
    if (instance == 0)
        return;

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

    fishPersBuffer = contextD3D12->createUploadBuffer(
        fishPers, contextD3D12->CalcConstantBufferByteSize(sizeof(FishPer) * instance));
    fishPersBufferView.BufferLocation = fishPersBuffer->GetGPUVirtualAddress();
    fishPersBufferView.SizeInBytes =
        contextD3D12->CalcConstantBufferByteSize(sizeof(FishPer) * instance);
    fishPersBufferView.StrideInBytes = sizeof(FishPer);

    vertexBufferView[5] = fishPersBufferView;

    inputElementDescs = {
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
         0},
        {"TEXCOORD", 3, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 4, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 5, DXGI_FORMAT_R32G32B32_FLOAT, 5, 0,
         D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
        {"TEXCOORD", 6, DXGI_FORMAT_R32_FLOAT, 5, 3 * sizeof(float),
         D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
        {"TEXCOORD", 7, DXGI_FORMAT_R32G32B32_FLOAT, 5, 4 * sizeof(float),
         D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
        {"TEXCOORD", 8, DXGI_FORMAT_R32_FLOAT, 5, 5 * sizeof(float),
         D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
    };

    // create constant buffer, desc.
    fishVertexBuffer = contextD3D12->createDefaultBuffer(
        &fishVertexUniforms, contextD3D12->CalcConstantBufferByteSize(sizeof(FishVertexUniforms)),
        fishVertexUploadBuffer);
    fishVertexView.BufferLocation = fishVertexBuffer->GetGPUVirtualAddress();
    fishVertexView.SizeInBytes    = contextD3D12->CalcConstantBufferByteSize(
        sizeof(fishVertexUniforms));  // CB size is required to be 256-byte aligned.
    contextD3D12->buildCbvDescriptor(fishVertexView, &fishVertexGPUHandle);
    lightFactorBuffer = contextD3D12->createDefaultBuffer(
        &lightFactorUniforms, contextD3D12->CalcConstantBufferByteSize(sizeof(LightFactorUniforms)),
        lightFactorUploadBuffer);
    lightFactorView.BufferLocation = lightFactorBuffer->GetGPUVirtualAddress();
    lightFactorView.SizeInBytes    = contextD3D12->CalcConstantBufferByteSize(
        sizeof(LightFactorUniforms));  // CB size is required to be 256-byte aligned.
    contextD3D12->buildCbvDescriptor(lightFactorView, &lightFactorGPUHandle);

    // Create root signature to bind resources.
    // Bind textures, samplers and immutable constant buffers in a descriptor table.
    // Bind frequently updated constant buffers by root descriptors.
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    CD3DX12_ROOT_PARAMETER1 rootParameters[4];
    CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
    rootParameters[0] = contextD3D12->rootParameterGeneral;
    rootParameters[1] = contextD3D12->rootParameterWorld;

    if (skyboxTexture && reflectionTexture)
    {
        diffuseTexture->createSrvDescriptor();
        normalTexture->createSrvDescriptor();
        reflectionTexture->createSrvDescriptor();

        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 2, 0, 2,
                       D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0, 2,
                       D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        rootParameters[2].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);
        rootParameters[3].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
    }
    else
    {
        diffuseTexture->createSrvDescriptor();
        normalTexture->createSrvDescriptor();

        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 2, 0, 2,
                       D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 2,
                       D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        rootParameters[2].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);
        rootParameters[3].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
    }

    rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 2u,
                               contextD3D12->staticSamplers.data(),
                               D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    contextD3D12->createRootSignature(rootSignatureDesc, m_rootSignature);

    contextD3D12->createGraphicsPipelineState(inputElementDescs, m_rootSignature,
                                              programD3D12->getVSModule(),
                                              programD3D12->getFSModule(), m_pipelineState, mBlend);

}

void FishModelInstancedDrawD3D12::prepareForDraw() const {}

void FishModelInstancedDrawD3D12::draw()
{
    if (instance == 0)
        return;

    CD3DX12_RANGE readRange(0, 0);
    UINT8 *m_pCbvDataBegin;
    fishPersBuffer->Map(0, &readRange, reinterpret_cast<void **>(&m_pCbvDataBegin));
    memcpy(m_pCbvDataBegin, fishPers, sizeof(FishPer) * instance);

    auto &commandList = contextD3D12->mCommandList;

    commandList->SetPipelineState(m_pipelineState.Get());
    commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    commandList->SetGraphicsRootDescriptorTable(0, contextD3D12->lightGPUHandle);
    commandList->SetGraphicsRootConstantBufferView(
        1, contextD3D12->lightWorldPositionView.BufferLocation);
    commandList->SetGraphicsRootDescriptorTable(2, fishVertexGPUHandle);
    commandList->SetGraphicsRootDescriptorTable(3, diffuseTexture->getTextureGPUHandle());

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, 6, vertexBufferView);
    commandList->IASetIndexBuffer(&indicesBuffer->mIndexBufferView);

    commandList->DrawIndexedInstanced(indicesBuffer->getTotalComponents(), instance, 0, 0, 0);
}

void FishModelInstancedDrawD3D12::updatePerInstanceUniforms(const WorldUniforms &worldUniforms) {}

void FishModelInstancedDrawD3D12::updateFishPerUniforms(float x,
                                                        float y,
                                                        float z,
                                                        float nextX,
                                                        float nextY,
                                                        float nextZ,
                                                        float scale,
                                                        float time,
                                                        int index)
{
    fishPers[index].worldPosition[0] = x;
    fishPers[index].worldPosition[1] = y;
    fishPers[index].worldPosition[2] = z;
    fishPers[index].nextPosition[0]  = nextX;
    fishPers[index].nextPosition[1]  = nextY;
    fishPers[index].nextPosition[2]  = nextZ;
    fishPers[index].scale            = scale;
    fishPers[index].time             = time;
}
