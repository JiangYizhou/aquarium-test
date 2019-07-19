//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// InnerModelDawn.cpp: Implements inner model of Dawn.

#include "InnerModelDawn.h"

InnerModelDawn::InnerModelDawn(const Context *context,
                               Aquarium *aquarium,
                               MODELGROUP type,
                               MODELNAME name,
                               bool blend)
    : Model(type, name, blend)
{
    mContextDawn = static_cast<const ContextDawn *>(context);

    mInnerUniforms.eta             = 1.0f;
    mInnerUniforms.tankColorFudge  = 0.796f;
    mInnerUniforms.refractionFudge = 3.0f;
}

InnerModelDawn::~InnerModelDawn()
{
    mPipeline         = nullptr;
    mGroupLayoutModel = nullptr;
    mGroupLayoutPer   = nullptr;
    mPipelineLayout   = nullptr;
    mBindGroupModel   = nullptr;
    mBindGroupPer     = nullptr;
    mInnerBuffer      = nullptr;
    mViewBuffer       = nullptr;
}

void InnerModelDawn::init()
{
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

    mGroupLayoutModel = mContextDawn->MakeBindGroupLayout({
        {0, dawn::ShaderStageBit::Fragment, dawn::BindingType::UniformBuffer},
        {1, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler},
        {2, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler},
        {3, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
        {4, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
        {5, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
        {6, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
    });

    mGroupLayoutPer = mContextDawn->MakeBindGroupLayout({
        {0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
    });

    mPipelineLayout = mContextDawn->MakeBasicPipelineLayout({
        mContextDawn->groupLayoutGeneral,
        mContextDawn->groupLayoutWorld,
        mGroupLayoutModel,
        mGroupLayoutPer,
    });

    mPipeline = mContextDawn->createRenderPipeline(mPipelineLayout, mProgramDawn,
                                                   mVertexInputDescriptor, mBlend);

    mInnerBuffer = mContextDawn->createBufferFromData(
        &mInnerUniforms, sizeof(mInnerUniforms),
        dawn::BufferUsageBit::CopyDst | dawn::BufferUsageBit::Uniform);
    mViewBuffer = mContextDawn->createBufferFromData(
        &mWorldUniformPer, sizeof(WorldUniforms),
        dawn::BufferUsageBit::CopyDst | dawn::BufferUsageBit::Uniform);

    mBindGroupModel =
        mContextDawn->makeBindGroup(mGroupLayoutModel, {{0, mInnerBuffer, 0, sizeof(InnerUniforms)},
                                                        {1, mReflectionTexture->getSampler()},
                                                        {2, mSkyboxTexture->getSampler()},
                                                        {3, mDiffuseTexture->getTextureView()},
                                                        {4, mNormalTexture->getTextureView()},
                                                        {5, mReflectionTexture->getTextureView()},
                                                        {6, mSkyboxTexture->getTextureView()}});

    mBindGroupPer =
        mContextDawn->makeBindGroup(mGroupLayoutPer, {
                                                         {0, mViewBuffer, 0, sizeof(WorldUniforms)},
                                                     });

    mContextDawn->setBufferData(mInnerBuffer, 0, sizeof(InnerUniforms), &mInnerUniforms);
}

void InnerModelDawn::prepareForDraw() const
{
}

void InnerModelDawn::draw()
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
    pass.SetVertexBuffers(3, 1, &mTangentBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(4, 1, &mBinormalBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetIndexBuffer(mIndicesBuffer->getBuffer(), 0);
    pass.DrawIndexed(mIndicesBuffer->getTotalComponents(), 1, 0, 0, 0);
}

void InnerModelDawn::updatePerInstanceUniforms(WorldUniforms *worldUniforms)
{
    std::memcpy(&mWorldUniformPer, worldUniforms, sizeof(WorldUniforms));

    mContextDawn->setBufferData(mViewBuffer, 0, sizeof(WorldUniforms), &mWorldUniformPer);
}
