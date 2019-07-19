//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// SeaweenModelDawn: Implements seaweed model of Dawn.

#include "SeaweedModelDawn.h"

SeaweedModelDawn::SeaweedModelDawn(const Context *context,
                                   Aquarium *aquarium,
                                   MODELGROUP type,
                                   MODELNAME name,
                                   bool blend)
    : SeaweedModel(type, name, blend), mInstance(0)
{
    mContextDawn = static_cast<const ContextDawn *>(context);
    mAquarium   = aquarium;

    mLightFactorUniforms.shininess      = 50.0f;
    mLightFactorUniforms.specularFactor = 1.0f;
}

SeaweedModelDawn::~SeaweedModelDawn()
{
    mPipeline          = nullptr;
    mGroupLayoutModel  = nullptr;
    mGroupLayoutPer    = nullptr;
    mPipelineLayout    = nullptr;
    mBindGroupModel    = nullptr;
    mBindGroupPer      = nullptr;
    mLightFactorBuffer = nullptr;
    mViewBuffer        = nullptr;
    mTimeBuffer        = nullptr;
}

void SeaweedModelDawn::init()
{
    mProgramDawn = static_cast<ProgramDawn *>(mProgram);

    mDiffuseTexture    = static_cast<TextureDawn *>(mTextureMap["diffuse"]);
    mNormalTexture     = static_cast<TextureDawn *>(mTextureMap["normalMap"]);
    mReflectionTexture = static_cast<TextureDawn *>(mTextureMap["reflectionMap"]);
    mSkyboxTexture     = static_cast<TextureDawn *>(mTextureMap["skybox"]);

    mPositionBuffer = static_cast<BufferDawn *>(mBufferMap["position"]);
    mNormalBuffer   = static_cast<BufferDawn *>(mBufferMap["normal"]);
    mTexCoordBuffer = static_cast<BufferDawn *>(mBufferMap["texCoord"]);
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
    mVertexInputDescriptor.bufferCount                   = 3;
    mVertexInputDescriptor.indexFormat                   = dawn::IndexFormat::Uint16;

    mGroupLayoutModel = mContextDawn->MakeBindGroupLayout({
        {0, dawn::ShaderStageBit::Fragment, dawn::BindingType::UniformBuffer},
        {1, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler},
        {2, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
    });

    mGroupLayoutPer = mContextDawn->MakeBindGroupLayout({
        {0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
        {1, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
    });

    mPipelineLayout = mContextDawn->MakeBasicPipelineLayout({
        mContextDawn->groupLayoutGeneral,
        mContextDawn->groupLayoutWorld,
        mGroupLayoutModel,
        mGroupLayoutPer,
    });

    mPipeline = mContextDawn->createRenderPipeline(mPipelineLayout, mProgramDawn,
                                                   mVertexInputDescriptor, mBlend);

    mLightFactorBuffer = mContextDawn->createBufferFromData(
        &mLightFactorUniforms, sizeof(mLightFactorUniforms),
        dawn::BufferUsageBit::CopyDst | dawn::BufferUsageBit::Uniform);
    mTimeBuffer = mContextDawn->createBufferFromData(
        &mSeaweedPer, sizeof(mSeaweedPer),
        dawn::BufferUsageBit::CopyDst | dawn::BufferUsageBit::Uniform);
    mViewBuffer = mContextDawn->createBufferFromData(
        &mWorldUniformPer, sizeof(WorldUniformPer),
        dawn::BufferUsageBit::CopyDst | dawn::BufferUsageBit::Uniform);

    mBindGroupModel = mContextDawn->makeBindGroup(
        mGroupLayoutModel, {
                               {0, mLightFactorBuffer, 0, sizeof(LightFactorUniforms)},
                               {1, mDiffuseTexture->getSampler()},
                               {2, mDiffuseTexture->getTextureView()},
                           });

    mBindGroupPer = mContextDawn->makeBindGroup(mGroupLayoutPer,
                                                {
                                                    {0, mViewBuffer, 0, sizeof(WorldUniformPer)},
                                                    {1, mTimeBuffer, 0, sizeof(SeaweedPer)},
                                                });

    mContextDawn->setBufferData(mLightFactorBuffer, 0, sizeof(mLightFactorUniforms),
                                &mLightFactorUniforms);
}

void SeaweedModelDawn::prepareForDraw() const
{
    mContextDawn->setBufferData(mViewBuffer, 0, sizeof(WorldUniformPer), &mWorldUniformPer);
    mContextDawn->setBufferData(mTimeBuffer, 0, sizeof(SeaweedPer), &mSeaweedPer);
}

void SeaweedModelDawn::draw()
{
    uint64_t vertexBufferOffsets[1] = {0};

    dawn::RenderPassEncoder pass = mContextDawn->getRenderPass();
    pass.SetPipeline(mPipeline);
    pass.SetBindGroup(0, mContextDawn->bindGroupGeneral, 0, nullptr);
    pass.SetBindGroup(1, mContextDawn->bindGroupWorld, 0, nullptr);
    pass.SetBindGroup(2, mBindGroupModel, 0, nullptr);
    pass.SetBindGroup(3, mBindGroupPer, 0, nullptr);
    pass.SetVertexBuffers(0, 1, &mPositionBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(1, 1, &mNormalBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(2, 1, &mTexCoordBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetIndexBuffer(mIndicesBuffer->getBuffer(), 0);
    pass.DrawIndexed(mIndicesBuffer->getTotalComponents(), mInstance, 0, 0, 0);
    mInstance = 0;
}

void SeaweedModelDawn::updatePerInstanceUniforms(WorldUniforms *worldUniforms)
{
    mWorldUniformPer.mWorldUniforms[mInstance] = *worldUniforms;
    mSeaweedPer.time[mInstance]                = mAquarium->g.mclock + mInstance;

    mInstance++;
}

void SeaweedModelDawn::updateSeaweedModelTime(float time)
{
}

