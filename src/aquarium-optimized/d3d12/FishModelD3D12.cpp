﻿//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelD3D12.cpp: Implements fish model of D3D12.

#include "BufferD3D12.h"
#include "FishModelD3D12.h"

FishModelD3D12::FishModelD3D12(Context *context,
                               Aquarium *aquarium,
                               MODELGROUP type,
                               MODELNAME name,
                               bool blend)
    : FishModel(type, name, blend), instance(0)
{
    mContextD3D12 = static_cast<ContextD3D12 *>(context);

    const Fish &fishInfo              = fishTable[name - MODELNAME::MODELSMALLFISHA];
    mFishVertexUniforms.fishLength     = fishInfo.fishLength;
    mFishVertexUniforms.fishBendAmount = fishInfo.fishBendAmount;
    mFishVertexUniforms.fishWaveLength = fishInfo.fishWaveLength;

    mLightFactorUniforms.shininess      = 5.0f;
    mLightFactorUniforms.specularFactor = 0.3f;

    instance = aquarium->fishCount[fishInfo.modelName - MODELNAME::MODELSMALLFISHA];
    mFishPers = new FishPer[instance];
}

FishModelD3D12::~FishModelD3D12()
{
    delete mFishPers;
}

void FishModelD3D12::init()
{
    if (instance == 0)
        return;

    mProgramD3D12 = static_cast<ProgramD3D12 *>(mProgram);

    mDiffuseTexture    = static_cast<TextureD3D12 *>(textureMap["diffuse"]);
    mNormalTexture     = static_cast<TextureD3D12 *>(textureMap["normalMap"]);
    mReflectionTexture = static_cast<TextureD3D12 *>(textureMap["reflectionMap"]);
    mSkyboxTexture     = static_cast<TextureD3D12 *>(textureMap["skybox"]);

    mPositionBuffer = static_cast<BufferD3D12 *>(bufferMap["position"]);
    mNormalBuffer   = static_cast<BufferD3D12 *>(bufferMap["normal"]);
    mTexCoordBuffer = static_cast<BufferD3D12 *>(bufferMap["texCoord"]);
    mTangentBuffer  = static_cast<BufferD3D12 *>(bufferMap["tangent"]);
    mBiNormalBuffer = static_cast<BufferD3D12 *>(bufferMap["binormal"]);
    mIndicesBuffer  = static_cast<BufferD3D12 *>(bufferMap["indices"]);

    mVertexBufferView[0] = mPositionBuffer->mVertexBufferView;
    mVertexBufferView[1] = mNormalBuffer->mVertexBufferView;
    mVertexBufferView[2] = mTexCoordBuffer->mVertexBufferView;
    mVertexBufferView[3] = mTangentBuffer->mVertexBufferView;
    mVertexBufferView[4] = mBiNormalBuffer->mVertexBufferView;

    mFishPersBuffer = mContextD3D12->createUploadBuffer(
        mFishPers, mContextD3D12->CalcConstantBufferByteSize(sizeof(FishPer) * instance));
    mFishPersBufferView.BufferLocation = mFishPersBuffer->GetGPUVirtualAddress();
    mFishPersBufferView.SizeInBytes    = mContextD3D12->CalcConstantBufferByteSize(sizeof(FishPer));

    mInputElementDescs = {
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
    };

    // create constant buffer, desc.
    mFishVertexBuffer = mContextD3D12->createDefaultBuffer(
        &mFishVertexUniforms, mContextD3D12->CalcConstantBufferByteSize(sizeof(FishVertexUniforms)),
        mFishVertexUploadBuffer);
    mFishVertexView.BufferLocation = mFishVertexBuffer->GetGPUVirtualAddress();
    mFishVertexView.SizeInBytes    = mContextD3D12->CalcConstantBufferByteSize(
        sizeof(mFishVertexUniforms));  // CB size is required to be 256-byte aligned.
    mContextD3D12->buildCbvDescriptor(mFishVertexView, &mFishVertexGPUHandle);
    mLightFactorBuffer = mContextD3D12->createDefaultBuffer(
        &mLightFactorUniforms,
        mContextD3D12->CalcConstantBufferByteSize(sizeof(LightFactorUniforms)),
        mLightFactorUploadBuffer);
    mLightFactorView.BufferLocation = mLightFactorBuffer->GetGPUVirtualAddress();
    mLightFactorView.SizeInBytes    = mContextD3D12->CalcConstantBufferByteSize(
        sizeof(LightFactorUniforms));  // CB size is required to be 256-byte aligned.
    mContextD3D12->buildCbvDescriptor(mLightFactorView, &mLightFactorGPUHandle);

    // Create root signature to bind resources.
    // Bind textures, samplers and immutable constant buffers in a descriptor table.
    // Bind frequently updated constant buffers by root descriptors.
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    CD3DX12_ROOT_PARAMETER1 rootParameters[5];
    CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
    rootParameters[0] = mContextD3D12->rootParameterGeneral;
    rootParameters[1] = mContextD3D12->rootParameterWorld;

    if (mSkyboxTexture && mReflectionTexture)
    {
        mDiffuseTexture->createSrvDescriptor();
        mNormalTexture->createSrvDescriptor();
        mReflectionTexture->createSrvDescriptor();

        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 2, 0, 2,
                       D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0, 2,
                       D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        rootParameters[2].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);
        rootParameters[3].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
    }
    else
    {
        mDiffuseTexture->createSrvDescriptor();
        mNormalTexture->createSrvDescriptor();

        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 2, 0, 2,
                       D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 2,
                       D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        rootParameters[2].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);
        rootParameters[3].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
    }

    rootParameters[4].InitAsConstantBufferView(0, 3, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC,
                                               D3D12_SHADER_VISIBILITY_VERTEX);

    rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 2u,
                               mContextD3D12->staticSamplers.data(),
                               D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    mContextD3D12->createRootSignature(rootSignatureDesc, mRootSignature);

    mContextD3D12->createGraphicsPipelineState(
        mInputElementDescs, mRootSignature, mProgramD3D12->getVSModule(),
        mProgramD3D12->getFSModule(), mPipelineState, mBlend);
}

void FishModelD3D12::prepareForDraw() const {}

void FishModelD3D12::draw()
{
    if (instance == 0)
        return;

    CD3DX12_RANGE readRange(0, 0);
    UINT8 *m_pCbvDataBegin;
    mFishPersBuffer->Map(0, &readRange, reinterpret_cast<void **>(&m_pCbvDataBegin));
    memcpy(m_pCbvDataBegin, mFishPers, sizeof(FishPer) * instance);

    auto &commandList = mContextD3D12->mCommandList;

    commandList->SetPipelineState(mPipelineState.Get());
    commandList->SetGraphicsRootSignature(mRootSignature.Get());

    commandList->SetGraphicsRootDescriptorTable(0, mContextD3D12->lightGPUHandle);
    commandList->SetGraphicsRootConstantBufferView(
        1, mContextD3D12->lightWorldPositionView.BufferLocation);
    commandList->SetGraphicsRootDescriptorTable(2, mFishVertexGPUHandle);
    commandList->SetGraphicsRootDescriptorTable(3, mDiffuseTexture->getTextureGPUHandle());

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, 5, mVertexBufferView);
    commandList->IASetIndexBuffer(&mIndicesBuffer->mIndexBufferView);

    for (int i = 0; i < instance; i++)
    {
        commandList->SetGraphicsRootConstantBufferView(
            4, mFishPersBufferView.BufferLocation + i * mFishPersBufferView.SizeInBytes);
        commandList->DrawIndexedInstanced(mIndicesBuffer->getTotalComponents(), 1, 0, 0, 0);
    }
}

void FishModelD3D12::updatePerInstanceUniforms(const WorldUniforms &worldUniforms) {}

void FishModelD3D12::updateFishPerUniforms(float x,
                                           float y,
                                           float z,
                                           float nextX,
                                           float nextY,
                                           float nextZ,
                                           float scale,
                                           float time,
                                           int index)
{
    mFishPers[index].worldPosition[0] = x;
    mFishPers[index].worldPosition[1] = y;
    mFishPers[index].worldPosition[2] = z;
    mFishPers[index].nextPosition[0]  = nextX;
    mFishPers[index].nextPosition[1]  = nextY;
    mFishPers[index].nextPosition[2]  = nextZ;
    mFishPers[index].scale            = scale;
    mFishPers[index].time             = time;
}
