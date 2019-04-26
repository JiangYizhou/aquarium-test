﻿//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelDawn.cpp: Implements fish model of Dawn.

#include "FishModelInstancedDrawDawn.h"
#include "BufferDawn.h"

FishModelInstancedDrawDawn::FishModelInstancedDrawDawn(const Context *context,
                                                       Aquarium *aquarium,
                                                       MODELGROUP type,
                                                       MODELNAME name,
                                                       bool blend)
    : FishModel(type, name, blend), instance(0)
{
    contextDawn = static_cast<const ContextDawn *>(context);

    lightFactorUniforms.shininess      = 5.0f;
    lightFactorUniforms.specularFactor = 0.3f;

    const Fish &fishInfo              = fishTable[name - MODELNAME::MODELSMALLFISHAINSTANCEDDRAWS];
    fishVertexUniforms.fishLength     = fishInfo.fishLength;
    fishVertexUniforms.fishBendAmount = fishInfo.fishBendAmount;
    fishVertexUniforms.fishWaveLength = fishInfo.fishWaveLength;

    instance = aquarium->fishCount[fishInfo.modelName - MODELNAME::MODELSMALLFISHA];
    fishPers = new FishPer[instance];
}

void FishModelInstancedDrawDawn::init()
{
    if (instance == 0)
        return;

    programDawn = static_cast<ProgramDawn *>(mProgram);

    diffuseTexture    = static_cast<TextureDawn *>(textureMap["diffuse"]);
    normalTexture     = static_cast<TextureDawn *>(textureMap["normalMap"]);
    reflectionTexture = static_cast<TextureDawn *>(textureMap["reflectionMap"]);
    skyboxTexture     = static_cast<TextureDawn *>(textureMap["skybox"]);

    positionBuffer = static_cast<BufferDawn *>(bufferMap["position"]);
    normalBuffer   = static_cast<BufferDawn *>(bufferMap["normal"]);
    texCoordBuffer = static_cast<BufferDawn *>(bufferMap["texCoord"]);
    tangentBuffer  = static_cast<BufferDawn *>(bufferMap["tangent"]);
    binormalBuffer = static_cast<BufferDawn *>(bufferMap["binormal"]);
    indicesBuffer  = static_cast<BufferDawn *>(bufferMap["indices"]);

    fishPersBuffer =
        contextDawn->createBuffer(sizeof(FishPer) * instance,
                                  dawn::BufferUsageBit::Vertex | dawn::BufferUsageBit::TransferDst);

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
            {5, 5, dawn::VertexFormat::Float3, offsetof(FishPer, worldPosition)},
            {6, 5, dawn::VertexFormat::Float2, offsetof(FishPer, scale)},
            {7, 5, dawn::VertexFormat::Float3, offsetof(FishPer, nextPosition)},
            {8, 5, dawn::VertexFormat::Float, offsetof(FishPer, time)},
        },
        {
            {0, positionBuffer->getDataSize(), dawn::InputStepMode::Vertex},
            {1, normalBuffer->getDataSize(), dawn::InputStepMode::Vertex},
            {2, texCoordBuffer->getDataSize(), dawn::InputStepMode::Vertex},
            {3, tangentBuffer->getDataSize(), dawn::InputStepMode::Vertex},
            {4, binormalBuffer->getDataSize(), dawn::InputStepMode::Vertex},
            {5, sizeof(FishPer), dawn::InputStepMode::Instance},
        });

    if (skyboxTexture && reflectionTexture)
    {
        groupLayoutModel = contextDawn->MakeBindGroupLayout({
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
        groupLayoutModel = contextDawn->MakeBindGroupLayout({
            {0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
            {1, dawn::ShaderStageBit::Fragment, dawn::BindingType::UniformBuffer},
            {2, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler},
            {3, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
            {4, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
        });
    }

    groupLayoutPer = contextDawn->MakeBindGroupLayout({
        {0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
    });

    pipelineLayout = contextDawn->MakeBasicPipelineLayout({
        contextDawn->groupLayoutGeneral,
        contextDawn->groupLayoutWorld,
        groupLayoutModel,
        groupLayoutPer,
    });

    pipeline = contextDawn->createRenderPipeline(pipelineLayout, programDawn, inputState, mBlend);

    fishVertexBuffer = contextDawn->createBufferFromData(
        &fishVertexUniforms, sizeof(FishVertexUniforms),
        dawn::BufferUsageBit::TransferDst | dawn::BufferUsageBit::Uniform);
    lightFactorBuffer = contextDawn->createBufferFromData(
        &lightFactorUniforms, sizeof(LightFactorUniforms),
        dawn::BufferUsageBit::TransferDst | dawn::BufferUsageBit::Uniform);

    // Fish models includes small, medium and big. Some of them contains reflection and skybox
    // texture, but some doesn't.
    if (skyboxTexture && reflectionTexture)
    {
        bindGroupModel = contextDawn->makeBindGroup(
            groupLayoutModel, {{0, fishVertexBuffer, 0, sizeof(FishVertexUniforms)},
                               {1, lightFactorBuffer, 0, sizeof(LightFactorUniforms)},
                               {2, reflectionTexture->getSampler()},
                               {3, skyboxTexture->getSampler()},
                               {4, diffuseTexture->getTextureView()},
                               {5, normalTexture->getTextureView()},
                               {6, reflectionTexture->getTextureView()},
                               {7, skyboxTexture->getTextureView()}});
    }
    else
    {
        bindGroupModel = contextDawn->makeBindGroup(
            groupLayoutModel, {{0, fishVertexBuffer, 0, sizeof(FishVertexUniforms)},
                               {1, lightFactorBuffer, 0, sizeof(LightFactorUniforms)},
                               {2, diffuseTexture->getSampler()},
                               {3, diffuseTexture->getTextureView()},
                               {4, normalTexture->getTextureView()}});
    }

    contextDawn->setBufferData(lightFactorBuffer, 0, sizeof(LightFactorUniforms),
                               &lightFactorUniforms);
    contextDawn->setBufferData(fishVertexBuffer, 0, sizeof(FishVertexUniforms),
                               &fishVertexUniforms);
}

void FishModelInstancedDrawDawn::preDraw() const {}

void FishModelInstancedDrawDawn::draw()
{
    if (instance == 0)
        return;

    uint64_t vertexBufferOffsets[1] = {0};

    contextDawn->setBufferData(fishPersBuffer, 0, sizeof(FishPer) * instance, fishPers);

    dawn::RenderPassEncoder pass = contextDawn->getRenderPass();
    pass.SetPipeline(pipeline);
    pass.SetBindGroup(0, contextDawn->bindGroupGeneral, 0, nullptr);
    pass.SetBindGroup(1, contextDawn->bindGroupWorld, 0, nullptr);
    pass.SetBindGroup(2, bindGroupModel, 0, nullptr);
    pass.SetVertexBuffers(0, 1, &positionBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(1, 1, &normalBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(2, 1, &texCoordBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(3, 1, &tangentBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(4, 1, &binormalBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(5, 1, &fishPersBuffer, vertexBufferOffsets);
    pass.SetIndexBuffer(indicesBuffer->getBuffer(), 0);
    pass.DrawIndexed(indicesBuffer->getTotalComponents(), instance, 0, 0, 0);
}

void FishModelInstancedDrawDawn::updatePerInstanceUniforms(WorldUniforms *worldUniforms) {}

void FishModelInstancedDrawDawn::updateFishPerUniforms(float x,
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

FishModelInstancedDrawDawn::~FishModelInstancedDrawDawn()
{
    inputState        = {};
    pipeline          = nullptr;
    groupLayoutModel  = nullptr;
    groupLayoutPer    = nullptr;
    pipelineLayout    = nullptr;
    bindGroupModel    = nullptr;
    bindGroupPer      = nullptr;
    fishVertexBuffer  = nullptr;
    lightFactorBuffer = nullptr;
    fishPersBuffer    = nullptr;
    delete fishPers;
}
