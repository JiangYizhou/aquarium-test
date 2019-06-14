//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// SeaweenModelDawn: Implements seaweed model of Dawn.

#include "SeaweedModelDawn.h"

SeaweedModelDawn::SeaweedModelDawn(const Context* context, Aquarium* aquarium, MODELGROUP type, MODELNAME name, bool blend)
    : SeaweedModel(type, name, blend), instance(0)
{
    contextDawn = static_cast<const ContextDawn*>(context);
    mAquarium   = aquarium;

    lightFactorUniforms.shininess = 50.0f;
    lightFactorUniforms.specularFactor = 1.0f;
}

SeaweedModelDawn::~SeaweedModelDawn()
{
    pipeline          = nullptr;
    groupLayoutModel  = nullptr;
    groupLayoutPer    = nullptr;
    pipelineLayout    = nullptr;
    bindGroupModel    = nullptr;
    bindGroupPer      = nullptr;
    lightFactorBuffer = nullptr;
    viewBuffer        = nullptr;
    timeBuffer        = nullptr;
}

void SeaweedModelDawn::init()
{
    programDawn = static_cast<ProgramDawn *>(mProgram);

    diffuseTexture = static_cast<TextureDawn*>(textureMap["diffuse"]);
    normalTexture = static_cast<TextureDawn*>(textureMap["normalMap"]);
    reflectionTexture = static_cast<TextureDawn*>(textureMap["reflectionMap"]);
    skyboxTexture = static_cast<TextureDawn*>(textureMap["skybox"]);

    positionBuffer = static_cast<BufferDawn*>(bufferMap["position"]);
    normalBuffer = static_cast<BufferDawn*>(bufferMap["normal"]);
    texCoordBuffer = static_cast<BufferDawn*>(bufferMap["texCoord"]);
    indicesBuffer = static_cast<BufferDawn*>(bufferMap["indices"]);

    vertexInputDescriptor.cBuffers[0].attributeCount    = 1;
    vertexInputDescriptor.cBuffers[0].stride            = positionBuffer->getDataSize();
    vertexInputDescriptor.cAttributes[0].format         = dawn::VertexFormat::Float3;
    vertexInputDescriptor.cAttributes[0].shaderLocation = 0;
    vertexInputDescriptor.cAttributes[0].offset         = 0;
    vertexInputDescriptor.cBuffers[0].attributes        = &vertexInputDescriptor.cAttributes[0];
    vertexInputDescriptor.cBuffers[1].attributeCount    = 1;
    vertexInputDescriptor.cBuffers[1].stride            = normalBuffer->getDataSize();
    vertexInputDescriptor.cAttributes[1].format         = dawn::VertexFormat::Float3;
    vertexInputDescriptor.cAttributes[1].shaderLocation = 1;
    vertexInputDescriptor.cAttributes[1].offset         = 0;
    vertexInputDescriptor.cBuffers[1].attributes        = &vertexInputDescriptor.cAttributes[1];
    vertexInputDescriptor.cBuffers[2].attributeCount    = 1;
    vertexInputDescriptor.cBuffers[2].stride            = texCoordBuffer->getDataSize();
    vertexInputDescriptor.cAttributes[2].format         = dawn::VertexFormat::Float2;
    vertexInputDescriptor.cAttributes[2].shaderLocation = 2;
    vertexInputDescriptor.cAttributes[2].offset         = 0;
    vertexInputDescriptor.cBuffers[2].attributes        = &vertexInputDescriptor.cAttributes[2];
    vertexInputDescriptor.bufferCount                   = 3;
    vertexInputDescriptor.indexFormat                   = dawn::IndexFormat::Uint16;

    groupLayoutModel = contextDawn->MakeBindGroupLayout({
        { 0, dawn::ShaderStageBit::Fragment, dawn::BindingType::UniformBuffer },
        { 1, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler },
        { 2, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture },
    });

    groupLayoutPer = contextDawn->MakeBindGroupLayout({
        { 0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
        { 1, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
    });

    pipelineLayout = contextDawn->MakeBasicPipelineLayout({ contextDawn->groupLayoutGeneral,
        contextDawn->groupLayoutWorld,
        groupLayoutModel,
        groupLayoutPer,
    });

    pipeline = contextDawn->createRenderPipeline(pipelineLayout, programDawn, vertexInputDescriptor,
                                                 mBlend);

    lightFactorBuffer = contextDawn->createBufferFromData(
        &lightFactorUniforms, sizeof(lightFactorUniforms),
        dawn::BufferUsageBit::TransferDst | dawn::BufferUsageBit::Uniform);
    timeBuffer = contextDawn->createBufferFromData(&seaweedPer, sizeof(seaweedPer), dawn::BufferUsageBit::TransferDst | dawn::BufferUsageBit::Uniform);
    viewBuffer = contextDawn->createBufferFromData(
        &worldUniformPer, sizeof(WorldUniformPer),
        dawn::BufferUsageBit::TransferDst | dawn::BufferUsageBit::Uniform);

    bindGroupModel = contextDawn->makeBindGroup(groupLayoutModel, {
        { 0, lightFactorBuffer, 0, sizeof(LightFactorUniforms) },
        { 1, diffuseTexture->getSampler() },
        { 2, diffuseTexture->getTextureView() },
    });

    bindGroupPer = contextDawn->makeBindGroup(groupLayoutPer, {
        { 0, viewBuffer, 0, sizeof(WorldUniformPer)},
        { 1, timeBuffer, 0, sizeof(SeaweedPer) },
    });

    contextDawn->setBufferData(lightFactorBuffer, 0, sizeof(lightFactorUniforms), &lightFactorUniforms);
}

void SeaweedModelDawn::preDraw() const
{
    contextDawn->setBufferData(viewBuffer, 0, sizeof(WorldUniformPer), &worldUniformPer);
    contextDawn->setBufferData(timeBuffer, 0, sizeof(SeaweedPer), &seaweedPer);
}

void SeaweedModelDawn::draw()
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
    pass.SetIndexBuffer(indicesBuffer->getBuffer(), 0);
    pass.DrawIndexed(indicesBuffer->getTotalComponents(), instance, 0, 0, 0);
    instance = 0;
}

void SeaweedModelDawn::updatePerInstanceUniforms(WorldUniforms *worldUniforms)
{
    worldUniformPer.worldUniforms[instance] = *worldUniforms;
    seaweedPer.time[instance]             = mAquarium->g.mclock + instance;

    instance++;
}

void SeaweedModelDawn::updateSeaweedModelTime(float time)
{
}

