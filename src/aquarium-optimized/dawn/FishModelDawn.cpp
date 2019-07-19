//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelDawn.cpp: Implements fish model of Dawn.

#include "BufferDawn.h" 
#include "FishModelDawn.h"

FishModelDawn::FishModelDawn(const Context *context,
                             Aquarium *aquarium,
                             MODELGROUP type,
                             MODELNAME name,
                             bool blend)
    : FishModel(type, name, blend), mInstance(0)
{
    mContextDawn = static_cast<const ContextDawn *>(context);

    mEnableDynamicBufferOffset =
        aquarium->toggleBitset.test(static_cast<size_t>(TOGGLE::ENABLEDYNAMICBUFFEROFFSET));

    mLightFactorUniforms.shininess      = 5.0f;
    mLightFactorUniforms.specularFactor = 0.3f;

    const Fish &fishInfo              = fishTable[name - MODELNAME::MODELSMALLFISHA];
    mFishVertexUniforms.fishLength     = fishInfo.fishLength;
    mFishVertexUniforms.fishBendAmount = fishInfo.fishBendAmount;
    mFishVertexUniforms.fishWaveLength = fishInfo.fishWaveLength;

    mInstance = aquarium->fishCount[fishInfo.modelName - MODELNAME::MODELSMALLFISHA];
    mFishPers = new FishPer[mInstance];
    if (mEnableDynamicBufferOffset)
    {
        mBindGroupPers = new dawn::BindGroup[1];
    }
    else
    {
        mBindGroupPers = new dawn::BindGroup[mInstance];
    }
}

void FishModelDawn::init()
{
    if (mInstance == 0)
        return;

    mProgramDawn = static_cast<ProgramDawn *>(mProgram);

    mDiffuseTexture    = static_cast<TextureDawn *>(mTextureMap["diffuse"]);
    mNormalTexture     = static_cast<TextureDawn *>(mTextureMap["normalMap"]);
    mReflectionTexture = static_cast<TextureDawn *>(mTextureMap["reflectionMap"]);
    mSkyboxTexture     = static_cast<TextureDawn *>(mTextureMap["skybox"]);

    mPositionBuffer = static_cast<BufferDawn *>(mBufferMap["position"]);
    mNormalBuffer   = static_cast<BufferDawn *>(mBufferMap["normal"]);
    mTexCoordBuffer = static_cast<BufferDawn *>(mBufferMap["texCoord"]);
    mTangentBuffer  = static_cast<BufferDawn *>(mBufferMap["tangent"]);
    mBinormalBuffer = static_cast<BufferDawn *>(mBufferMap["binormal"]);
    mIndicesBuffer  = static_cast<BufferDawn *>(mBufferMap["indices"]);

    mVertexInputDescriptor.cBuffers[0].attributeCount    = 1;
    mVertexInputDescriptor.cBuffers[0].stride            = mPositionBuffer->getDataSize();
    mVertexInputDescriptor.cAttributes[0].format         = dawn::VertexFormat::Float3;
    mVertexInputDescriptor.cAttributes[0].shaderLocation = 0;
    mVertexInputDescriptor.cAttributes[0].offset         = 0;
    mVertexInputDescriptor.cBuffers[0].attributes        = &mVertexInputDescriptor.cAttributes[0];
    mVertexInputDescriptor.cBuffers[1].attributeCount    = 1;
    mVertexInputDescriptor.cBuffers[1].stride            = mNormalBuffer->getDataSize();
    mVertexInputDescriptor.cAttributes[1].format         = dawn::VertexFormat::Float3;
    mVertexInputDescriptor.cAttributes[1].shaderLocation = 1;
    mVertexInputDescriptor.cAttributes[1].offset         = 0;
    mVertexInputDescriptor.cBuffers[1].attributes        = &mVertexInputDescriptor.cAttributes[1];
    mVertexInputDescriptor.cBuffers[2].attributeCount    = 1;
    mVertexInputDescriptor.cBuffers[2].stride            = mTexCoordBuffer->getDataSize();
    mVertexInputDescriptor.cAttributes[2].format         = dawn::VertexFormat::Float2;
    mVertexInputDescriptor.cAttributes[2].shaderLocation = 2;
    mVertexInputDescriptor.cAttributes[2].offset         = 0;
    mVertexInputDescriptor.cBuffers[2].attributes        = &mVertexInputDescriptor.cAttributes[2];
    mVertexInputDescriptor.cBuffers[3].attributeCount    = 1;
    mVertexInputDescriptor.cBuffers[3].stride            = mTangentBuffer->getDataSize();
    mVertexInputDescriptor.cAttributes[3].format         = dawn::VertexFormat::Float3;
    mVertexInputDescriptor.cAttributes[3].shaderLocation = 3;
    mVertexInputDescriptor.cAttributes[3].offset         = 0;
    mVertexInputDescriptor.cBuffers[3].attributes        = &mVertexInputDescriptor.cAttributes[3];
    mVertexInputDescriptor.cBuffers[4].attributeCount    = 1;
    mVertexInputDescriptor.cBuffers[4].stride            = mBinormalBuffer->getDataSize();
    mVertexInputDescriptor.cAttributes[4].format         = dawn::VertexFormat::Float3;
    mVertexInputDescriptor.cAttributes[4].shaderLocation = 4;
    mVertexInputDescriptor.cAttributes[4].offset         = 0;
    mVertexInputDescriptor.cBuffers[4].attributes        = &mVertexInputDescriptor.cAttributes[4];
    mVertexInputDescriptor.bufferCount                   = 5;
    mVertexInputDescriptor.indexFormat                   = dawn::IndexFormat::Uint16;

    if (mSkyboxTexture && mReflectionTexture)
    {
        mGroupLayoutModel = mContextDawn->MakeBindGroupLayout({
            {0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
            {1, dawn::ShaderStageBit::Fragment, dawn::BindingType::UniformBuffer},
            {2, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler},
            {3, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler},
            {4, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
            {5, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
            {6, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
            {7, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
        });
    }
    else
    {
        mGroupLayoutModel = mContextDawn->MakeBindGroupLayout({
            {0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
            {1, dawn::ShaderStageBit::Fragment, dawn::BindingType::UniformBuffer},
            {2, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler},
            {3, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
            {4, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
        });
    }

    if (mEnableDynamicBufferOffset)
    {
        mGroupLayoutPer = mContextDawn->MakeBindGroupLayout({
            {0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer, true},
        });
    }
    else
    {
        mGroupLayoutPer = mContextDawn->MakeBindGroupLayout({
            {0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
        });
    }

    mPipelineLayout = mContextDawn->MakeBasicPipelineLayout({
        mContextDawn->groupLayoutGeneral,
        mContextDawn->groupLayoutWorld,
        mGroupLayoutModel,
        mGroupLayoutPer,
    });

    mPipeline = mContextDawn->createRenderPipeline(mPipelineLayout, mProgramDawn,
                                                   mVertexInputDescriptor, mBlend);

    mFishVertexBuffer = mContextDawn->createBufferFromData(
        &mFishVertexUniforms, sizeof(mFishVertexUniforms),
        dawn::BufferUsageBit::CopyDst | dawn::BufferUsageBit::Uniform);
    mLightFactorBuffer = mContextDawn->createBufferFromData(
        &mLightFactorUniforms, sizeof(LightFactorUniforms),
        dawn::BufferUsageBit::CopyDst | dawn::BufferUsageBit::Uniform);
    mFishPersBuffer = mContextDawn->createBufferFromData(
        mFishPers, sizeof(FishPer) * mInstance,
        dawn::BufferUsageBit::CopyDst | dawn::BufferUsageBit::Uniform);

    // Fish models includes small, medium and big. Some of them contains reflection and skybox
    // texture, but some doesn't.
    if (mSkyboxTexture && mReflectionTexture)
    {
        mBindGroupModel = mContextDawn->makeBindGroup(
            mGroupLayoutModel, {{0, mFishVertexBuffer, 0, sizeof(mFishVertexUniforms)},
                                {1, mLightFactorBuffer, 0, sizeof(LightFactorUniforms)},
                                {2, mReflectionTexture->getSampler()},
                                {3, mSkyboxTexture->getSampler()},
                                {4, mDiffuseTexture->getTextureView()},
                                {5, mNormalTexture->getTextureView()},
                                {6, mReflectionTexture->getTextureView()},
                                {7, mSkyboxTexture->getTextureView()}});
    }
    else
    {
        mBindGroupModel = mContextDawn->makeBindGroup(
            mGroupLayoutModel, {{0, mFishVertexBuffer, 0, sizeof(mFishVertexUniforms)},
                                {1, mLightFactorBuffer, 0, sizeof(LightFactorUniforms)},
                                {2, mDiffuseTexture->getSampler()},
                                {3, mDiffuseTexture->getTextureView()},
                                {4, mNormalTexture->getTextureView()}});
    }

    if (mEnableDynamicBufferOffset)
    {
        mBindGroupPers[0] = mContextDawn->makeBindGroup(mGroupLayoutPer,
                                                        {{0, mFishPersBuffer, 0, sizeof(FishPer)}});
    }
    else
    {
        for (int i = 0; i < mInstance; i++)
        {
            mBindGroupPers[i] = mContextDawn->makeBindGroup(
                mGroupLayoutPer, {{0, mFishPersBuffer, sizeof(FishPer) * i, sizeof(FishPer)}});
        }
    }

    mContextDawn->setBufferData(mLightFactorBuffer, 0, sizeof(LightFactorUniforms),
                                &mLightFactorUniforms);
    mContextDawn->setBufferData(mFishVertexBuffer, 0, sizeof(mFishVertexUniforms),
                                &mFishVertexUniforms);
}

void FishModelDawn::prepareForDraw() const {}

void FishModelDawn::draw()
{
    if (mInstance == 0)
        return;

    uint64_t vertexBufferOffsets[1] = {0};

    mContextDawn->setBufferData(mFishPersBuffer, 0, sizeof(FishPer) * mInstance, mFishPers);

    dawn::RenderPassEncoder pass = mContextDawn->getRenderPass();
    pass.SetPipeline(mPipeline);
    pass.SetBindGroup(0, mContextDawn->bindGroupGeneral, 0, nullptr);
    pass.SetBindGroup(1, mContextDawn->bindGroupWorld, 0, nullptr);
    pass.SetBindGroup(2, mBindGroupModel, 0, nullptr);
    pass.SetVertexBuffers(0, 1, &mPositionBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(1, 1, &mNormalBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(2, 1, &mTexCoordBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(3, 1, &mTangentBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(4, 1, &mBinormalBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetIndexBuffer(mIndicesBuffer->getBuffer(), 0);

    if (mEnableDynamicBufferOffset)
    {
        for (int i = 0; i < mInstance; i++)
        {
            uint64_t offset = 256u * i;
            pass.SetBindGroup(3, mBindGroupPers[0], 1, &offset);
            pass.DrawIndexed(mIndicesBuffer->getTotalComponents(), 1, 0, 0, 0);
        }
    }
    else
    {
        for (int i = 0; i < mInstance; i++)
        {
            pass.SetBindGroup(3, mBindGroupPers[i], 0, nullptr);
            pass.DrawIndexed(mIndicesBuffer->getTotalComponents(), 1, 0, 0, 0);
        }
    }
}

void FishModelDawn::updatePerInstanceUniforms(WorldUniforms *worldUniforms) {}

void FishModelDawn::updateFishPerUniforms(float x,
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

FishModelDawn::~FishModelDawn()
{
    mPipeline          = nullptr;
    mGroupLayoutModel  = nullptr;
    mGroupLayoutPer    = nullptr;
    mPipelineLayout    = nullptr;
    mBindGroupModel    = nullptr;
    mFishVertexBuffer  = nullptr;
    mLightFactorBuffer = nullptr;
    mFishPersBuffer    = nullptr;
    delete mFishPers;
    if (mEnableDynamicBufferOffset)
    {
        mBindGroupPers[0] = nullptr;
    }
    else
    {
        for (int i = 0; i < mInstance; i++)
        {
            mBindGroupPers[i] = nullptr;
        }
    }

    mBindGroupPers = nullptr;
}
