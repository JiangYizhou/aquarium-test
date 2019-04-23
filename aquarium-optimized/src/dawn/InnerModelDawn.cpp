//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// InnerModelDawn.cpp: Implements inner model of Dawn.

#include <cstring>

#include "InnerModelDawn.h"

InnerModelDawn::InnerModelDawn(const Context* context, Aquarium* aquarium, MODELGROUP type, MODELNAME name, bool blend)
    : InnerModel(type, name, blend)
{
    contextDawn = static_cast<const ContextDawn*>(context);

    innerUniforms.eta = 1.0f;
    innerUniforms.tankColorFudge = 0.796f;
    innerUniforms.refractionFudge = 3.0f;
}

InnerModelDawn::~InnerModelDawn()
{
    inputState       = {};
    pipeline         = nullptr;
    groupLayoutModel = nullptr;
    groupLayoutPer   = nullptr;
    pipelineLayout   = nullptr;
    bindGroupModel   = nullptr;
    bindGroupPer     = nullptr;
    innerBuffer      = nullptr;
    viewBuffer       = nullptr;
}

void InnerModelDawn::init()
{
    programDawn = static_cast<ProgramDawn *>(mProgram);

    diffuseTexture = static_cast<TextureDawn*>(textureMap["diffuse"]);
    normalTexture = static_cast<TextureDawn*>(textureMap["normalMap"]);
    reflectionTexture = static_cast<TextureDawn*>(textureMap["reflectionMap"]);
    skyboxTexture = static_cast<TextureDawn*>(textureMap["skybox"]);

    positionBuffer = static_cast<BufferDawn*>(bufferMap["position"]);
    normalBuffer = static_cast<BufferDawn*>(bufferMap["normal"]);
    texCoordBuffer = static_cast<BufferDawn*>(bufferMap["texCoord"]);
    tangentBuffer = static_cast<BufferDawn*>(bufferMap["tangent"]);
    binormalBuffer = static_cast<BufferDawn*>(bufferMap["binormal"]);
    indicesBuffer = static_cast<BufferDawn*>(bufferMap["indices"]);

    std::vector<dawn::VertexAttributeDescriptor> vertexAttributeDescriptor;
    std::vector<dawn::VertexInputDescriptor> vertexInputDescriptor;
    contextDawn->createInputState(
        &inputState, vertexAttributeDescriptor, vertexInputDescriptor,
        {
            {0, 0, dawn::VertexFormat::Float3, 0},
            {1, 1, dawn::VertexFormat::Float3, 0},
            {2, 2, dawn::VertexFormat::Float2, 0},
            {3, 3, dawn::VertexFormat::Float3, 0},
            {4, 4, dawn::VertexFormat::Float3, 0},
        },
        {{0, positionBuffer->getDataSize(), dawn::InputStepMode::Vertex},
         {1, normalBuffer->getDataSize(), dawn::InputStepMode::Vertex},
         {2, texCoordBuffer->getDataSize(), dawn::InputStepMode::Vertex},
         {3, tangentBuffer->getDataSize(), dawn::InputStepMode::Vertex},
         {4, binormalBuffer->getDataSize(), dawn::InputStepMode::Vertex}});

    groupLayoutModel = contextDawn->MakeBindGroupLayout({
       { 0, dawn::ShaderStageBit::Fragment, dawn::BindingType::UniformBuffer },
       { 1, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler },
       { 2, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler },
       { 3, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture },
       { 4, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture },
       { 5, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture },
       { 6, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture },
    });

    groupLayoutPer = contextDawn->MakeBindGroupLayout({
        {0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
    });
    
    pipelineLayout = contextDawn->MakeBasicPipelineLayout({ contextDawn->groupLayoutGeneral,
        contextDawn->groupLayoutWorld,
        groupLayoutModel,
        groupLayoutPer,
    });

    pipeline = contextDawn->createRenderPipeline(pipelineLayout, programDawn, inputState, mBlend);

    innerBuffer = contextDawn->createBufferFromData(&innerUniforms, sizeof(innerUniforms), dawn::BufferUsageBit::TransferDst | dawn::BufferUsageBit::Uniform);
    viewBuffer = contextDawn->createBufferFromData(
        &worldUniformPer, sizeof(WorldUniforms),
        dawn::BufferUsageBit::TransferDst | dawn::BufferUsageBit::Uniform);

    bindGroupModel = contextDawn->makeBindGroup(groupLayoutModel, {
        { 0, innerBuffer, 0, sizeof(InnerUniforms) },
        { 1, reflectionTexture->getSampler() },
        { 2, skyboxTexture->getSampler() },
        { 3, diffuseTexture->getTextureView() },
        { 4, normalTexture->getTextureView() },
        { 5, reflectionTexture->getTextureView() },
        { 6, skyboxTexture->getTextureView() }
    });

    bindGroupPer =
        contextDawn->makeBindGroup(groupLayoutPer, {
                                                       {0, viewBuffer, 0, sizeof(WorldUniforms)},
                                                   });

    contextDawn->setBufferData(innerBuffer, 0, sizeof(InnerUniforms), &innerUniforms);
}

void InnerModelDawn::preDraw() const
{
}

void InnerModelDawn::draw()
{
    uint64_t vertexBufferOffsets[1] = {0};

    dawn::RenderPassEncoder pass = contextDawn->getRenderPass();
    pass.SetPipeline(pipeline);
    pass.SetBindGroup(0, contextDawn->bindGroupGeneral, 0, nullptr);
    pass.SetBindGroup(1, contextDawn->bindGroupWorld, 0, nullptr);
    pass.SetBindGroup(2, bindGroupModel, 0, nullptr);
    pass.SetBindGroup(3, bindGroupPer, 0, nullptr);
    pass.SetVertexBuffers(0, 1, &positionBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(1, 1, &normalBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(2, 1, &texCoordBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(3, 1, &tangentBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(4, 1, &binormalBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetIndexBuffer(indicesBuffer->getBuffer(), 0);
    pass.DrawIndexed(indicesBuffer->getTotalComponents(), 1, 0, 0, 0);
}

void InnerModelDawn::updatePerInstanceUniforms(WorldUniforms* worldUniforms)
{
    std::memcpy(&worldUniformPer, worldUniforms, sizeof(WorldUniforms));

    contextDawn->setBufferData(viewBuffer, 0, sizeof(WorldUniforms), &worldUniformPer);
}
