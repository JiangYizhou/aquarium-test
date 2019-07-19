﻿//
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
    : FishModel(type, name, blend), mInstance(0)
{
    mContextD3D12 = static_cast<ContextD3D12 *>(context);

    const Fish &fishInfo              = fishTable[name - MODELNAME::MODELSMALLFISHAINSTANCEDDRAWS];
    mFishVertexUniforms.fishLength     = fishInfo.fishLength;
    mFishVertexUniforms.fishBendAmount = fishInfo.fishBendAmount;
    mFishVertexUniforms.fishWaveLength = fishInfo.fishWaveLength;

    mLightFactorUniforms.shininess      = 5.0f;
    mLightFactorUniforms.specularFactor = 0.3f;

    mInstance = aquarium->fishCount[fishInfo.modelName - MODELNAME::MODELSMALLFISHA];
    mFishPers = new FishPer[mInstance];
}

FishModelInstancedDrawD3D12::~FishModelInstancedDrawD3D12()
{
    delete mFishPers;
}

void FishModelInstancedDrawD3D12::init()
{
    if (mInstance == 0)
        return;

    mProgramD3D12 = static_cast<ProgramD3D12 *>(mProgram);

    mDiffuseTexture    = static_cast<TextureD3D12 *>(mTextureMap["diffuse"]);
    mNormalTexture     = static_cast<TextureD3D12 *>(mTextureMap["normalMap"]);
    mReflectionTexture = static_cast<TextureD3D12 *>(mTextureMap["reflectionMap"]);
    mSkyboxTexture     = static_cast<TextureD3D12 *>(mTextureMap["skybox"]);

    mPositionBuffer = static_cast<BufferD3D12 *>(mBufferMap["position"]);
    mNormalBuffer   = static_cast<BufferD3D12 *>(mBufferMap["normal"]);
    mTexCoordBuffer = static_cast<BufferD3D12 *>(mBufferMap["texCoord"]);
    mTangentBuffer  = static_cast<BufferD3D12 *>(mBufferMap["tangent"]);
    mBinormalBuffer = static_cast<BufferD3D12 *>(mBufferMap["binormal"]);
    mIndicesBuffer  = static_cast<BufferD3D12 *>(mBufferMap["indices"]);

    mVertexBufferView[0] = mPositionBuffer->mVertexBufferView;
    mVertexBufferView[1] = mNormalBuffer->mVertexBufferView;
    mVertexBufferView[2] = mTexCoordBuffer->mVertexBufferView;
    mVertexBufferView[3] = mTangentBuffer->mVertexBufferView;
    mVertexBufferView[4] = mBinormalBuffer->mVertexBufferView;

    mFishPersBuffer = mContextD3D12->createUploadBuffer(
        mFishPers, mContextD3D12->CalcConstantBufferByteSize(sizeof(FishPer) * mInstance));
    mFishPersBufferView.BufferLocation = mFishPersBuffer->GetGPUVirtualAddress();
    mFishPersBufferView.SizeInBytes =
        mContextD3D12->CalcConstantBufferByteSize(sizeof(FishPer) * mInstance);
    mFishPersBufferView.StrideInBytes = sizeof(FishPer);

    mVertexBufferView[5] = mFishPersBufferView;

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
    mFishVertexBuffer = mContextD3D12->createDefaultBuffer(
        &mFishVertexUniforms,
        mContextD3D12->CalcConstantBufferByteSize(sizeof(mFishVertexUniforms)),
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
    CD3DX12_ROOT_PARAMETER1 rootParameters[4];
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

    rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 2u,
                               mContextD3D12->staticSamplers.data(),
                               D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    mContextD3D12->createRootSignature(&rootSignatureDesc, mRootSignature);

    mContextD3D12->createGraphicsPipelineState(
        mInputElementDescs, mRootSignature, mProgramD3D12->getVSModule(),
        mProgramD3D12->getFSModule(), mPipelineState, mBlend);
}

void FishModelInstancedDrawD3D12::prepareForDraw() const {}

void FishModelInstancedDrawD3D12::draw()
{
    if (mInstance == 0)
        return;

    CD3DX12_RANGE readRange(0, 0);
    UINT8 *m_pCbvDataBegin;
    mFishPersBuffer->Map(0, &readRange, reinterpret_cast<void **>(&m_pCbvDataBegin));
    memcpy(m_pCbvDataBegin, mFishPers, sizeof(FishPer) * mInstance);

    auto &commandList = mContextD3D12->mCommandList;

    commandList->SetPipelineState(mPipelineState.Get());
    commandList->SetGraphicsRootSignature(mRootSignature.Get());

    commandList->SetGraphicsRootDescriptorTable(0, mContextD3D12->lightGPUHandle);
    commandList->SetGraphicsRootConstantBufferView(
        1, mContextD3D12->lightWorldPositionView.BufferLocation);
    commandList->SetGraphicsRootDescriptorTable(2, mFishVertexGPUHandle);
    commandList->SetGraphicsRootDescriptorTable(3, mDiffuseTexture->getTextureGPUHandle());

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, 6, mVertexBufferView);
    commandList->IASetIndexBuffer(&mIndicesBuffer->mIndexBufferView);

    commandList->DrawIndexedInstanced(mIndicesBuffer->getTotalComponents(), mInstance, 0, 0, 0);
}

void FishModelInstancedDrawD3D12::updatePerInstanceUniforms(WorldUniforms *worldUniforms) {}

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
    mFishPers[index].worldPosition[0] = x;
    mFishPers[index].worldPosition[1] = y;
    mFishPers[index].worldPosition[2] = z;
    mFishPers[index].nextPosition[0]  = nextX;
    mFishPers[index].nextPosition[1]  = nextY;
    mFishPers[index].nextPosition[2]  = nextZ;
    mFishPers[index].scale            = scale;
    mFishPers[index].time             = time;
}
