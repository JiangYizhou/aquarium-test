//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#include "OutsideModelD3D12.h"

OutsideModelD3D12::OutsideModelD3D12(Context *context,
                                     Aquarium *aquarium,
                                     MODELGROUP type,
                                     MODELNAME name,
                                     bool blend)
    : Model(type, name, blend)
{
    mContextD3D12 = static_cast<ContextD3D12 *>(context);

    mLightFactorUniforms.shininess      = 50.0f;
    mLightFactorUniforms.specularFactor = 0.0f;
}

void OutsideModelD3D12::init()
{
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

    mInputElementDescs = {
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
         0},
    };

    // create constant buffer, desc.
    mLightFactorBuffer = mContextD3D12->createDefaultBuffer(
        &mLightFactorUniforms,
        mContextD3D12->CalcConstantBufferByteSize(sizeof(LightFactorUniforms)),
        mLightFactorUploadBuffer);
    mLightFactorView.BufferLocation = mLightFactorBuffer->GetGPUVirtualAddress();
    mLightFactorView.SizeInBytes    = mContextD3D12->CalcConstantBufferByteSize(
        sizeof(LightFactorUniforms));  // CB size is required to be 256-byte aligned.
    mContextD3D12->buildCbvDescriptor(mLightFactorView, &mLightFactorGPUHandle);
    mWorldBuffer = mContextD3D12->createUploadBuffer(
        &mWorldUniformPer, mContextD3D12->CalcConstantBufferByteSize(sizeof(WorldUniforms)));
    mWorldBufferView.BufferLocation = mWorldBuffer->GetGPUVirtualAddress();
    mWorldBufferView.SizeInBytes = mContextD3D12->CalcConstantBufferByteSize(sizeof(WorldUniforms));

    // Create root signature to bind resources.
    // Bind textures, samplers and immutable constant buffers in a descriptor table.
    // Bind frequently updated constant buffers by root descriptors.
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    CD3DX12_ROOT_PARAMETER1 rootParameters[5];
    CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
    rootParameters[0] = mContextD3D12->rootParameterGeneral;
    rootParameters[1] = mContextD3D12->rootParameterWorld;

    mDiffuseTexture->createSrvDescriptor();

    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 2,
                   D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 2,
                   D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    rootParameters[2].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[3].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);

    rootParameters[4].InitAsConstantBufferView(0, 3, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC,
                                               D3D12_SHADER_VISIBILITY_VERTEX);

    rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 2u,
                               mContextD3D12->staticSamplers.data(),
                               D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    mContextD3D12->createRootSignature(&rootSignatureDesc, mRootSignature);

    mContextD3D12->createGraphicsPipelineState(
        mInputElementDescs, mRootSignature, mProgramD3D12->getVSModule(),
        mProgramD3D12->getFSModule(), mPipelineState, mBlend);
}

void OutsideModelD3D12::prepareForDraw() const {}

void OutsideModelD3D12::draw()
{
    auto &commandList = mContextD3D12->mCommandList;

    commandList->SetPipelineState(mPipelineState.Get());
    commandList->SetGraphicsRootSignature(mRootSignature.Get());

    commandList->SetGraphicsRootDescriptorTable(0, mContextD3D12->lightGPUHandle);
    commandList->SetGraphicsRootConstantBufferView(
        1, mContextD3D12->lightWorldPositionView.BufferLocation);
    commandList->SetGraphicsRootDescriptorTable(2, mLightFactorGPUHandle);
    commandList->SetGraphicsRootDescriptorTable(3, mDiffuseTexture->getTextureGPUHandle());
    commandList->SetGraphicsRootConstantBufferView(4, mWorldBufferView.BufferLocation);

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, 3, mVertexBufferView);

    commandList->IASetIndexBuffer(&mIndicesBuffer->mIndexBufferView);

    commandList->DrawIndexedInstanced(mIndicesBuffer->getTotalComponents(), 1, 0, 0, 0);
}

void OutsideModelD3D12::updatePerInstanceUniforms(WorldUniforms *worldUniforms)
{
    memcpy(&mWorldUniformPer, worldUniforms, sizeof(WorldUniforms));

    CD3DX12_RANGE readRange(0, 0);
    UINT8 *m_pCbvDataBegin;
    mWorldBuffer->Map(0, &readRange, reinterpret_cast<void **>(&m_pCbvDataBegin));
    memcpy(m_pCbvDataBegin, &mWorldUniformPer, sizeof(WorldUniforms));
}
