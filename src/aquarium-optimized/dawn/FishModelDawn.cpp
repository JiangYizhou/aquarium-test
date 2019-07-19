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
    : FishModel(type, name, blend), instance(0)
{
    contextDawn = static_cast<const ContextDawn *>(context);

    enableDynamicBufferOffset =
        aquarium->toggleBitset.test(static_cast<size_t>(TOGGLE::ENABLEDYNAMICBUFFEROFFSET));

    lightFactorUniforms.shininess      = 5.0f;
    lightFactorUniforms.specularFactor = 0.3f;

    const Fish &fishInfo              = fishTable[name - MODELNAME::MODELSMALLFISHA];
    fishVertexUniforms.fishLength     = fishInfo.fishLength;
    fishVertexUniforms.fishBendAmount = fishInfo.fishBendAmount;
    fishVertexUniforms.fishWaveLength = fishInfo.fishWaveLength;

    instance      = aquarium->fishCount[fishInfo.modelName - MODELNAME::MODELSMALLFISHA];
    fishPers      = new FishPer[instance];
    if (enableDynamicBufferOffset)
    {
        bindGroupPers = new dawn::BindGroup[1];
    }
    else
    {
        bindGroupPers = new dawn::BindGroup[instance];
    }
}

void FishModelDawn::init()
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
    vertexInputDescriptor.cBuffers[3].attributeCount    = 1;
    vertexInputDescriptor.cBuffers[3].stride            = tangentBuffer->getDataSize();
    vertexInputDescriptor.cAttributes[3].format         = dawn::VertexFormat::Float3;
    vertexInputDescriptor.cAttributes[3].shaderLocation = 3;
    vertexInputDescriptor.cAttributes[3].offset         = 0;
    vertexInputDescriptor.cBuffers[3].attributes        = &vertexInputDescriptor.cAttributes[3];
    vertexInputDescriptor.cBuffers[4].attributeCount    = 1;
    vertexInputDescriptor.cBuffers[4].stride            = binormalBuffer->getDataSize();
    vertexInputDescriptor.cAttributes[4].format         = dawn::VertexFormat::Float3;
    vertexInputDescriptor.cAttributes[4].shaderLocation = 4;
    vertexInputDescriptor.cAttributes[4].offset         = 0;
    vertexInputDescriptor.cBuffers[4].attributes        = &vertexInputDescriptor.cAttributes[4];
    vertexInputDescriptor.bufferCount                   = 5;
    vertexInputDescriptor.indexFormat                   = dawn::IndexFormat::Uint16;

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

    if (enableDynamicBufferOffset)
    {
        groupLayoutPer = contextDawn->MakeBindGroupLayout({
            {0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer, true},
        });
    }
    else
    {
        groupLayoutPer = contextDawn->MakeBindGroupLayout({
            {0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
        });
    }

    pipelineLayout = contextDawn->MakeBasicPipelineLayout({
        contextDawn->groupLayoutGeneral,
        contextDawn->groupLayoutWorld,
        groupLayoutModel,
        groupLayoutPer,
    });

    pipeline = contextDawn->createRenderPipeline(pipelineLayout, programDawn, vertexInputDescriptor,
                                                 mBlend);

    fishVertexBuffer = contextDawn->createBufferFromData(
        &fishVertexUniforms, sizeof(FishVertexUniforms),
        dawn::BufferUsageBit::CopyDst | dawn::BufferUsageBit::Uniform);
    lightFactorBuffer = contextDawn->createBufferFromData(
        &lightFactorUniforms, sizeof(LightFactorUniforms),
        dawn::BufferUsageBit::CopyDst | dawn::BufferUsageBit::Uniform);
    fishPersBuffer = contextDawn->createBufferFromData(
        fishPers, sizeof(FishPer) * instance,
        dawn::BufferUsageBit::CopyDst | dawn::BufferUsageBit::Uniform);

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

    if (enableDynamicBufferOffset)
    {
        bindGroupPers[0] = contextDawn->makeBindGroup(
            groupLayoutPer, {{0, fishPersBuffer, 0, sizeof(FishPer)}});
    }
    else
    {
        for (int i = 0; i < instance; i++)
        {
            bindGroupPers[i] = contextDawn->makeBindGroup(
                groupLayoutPer, {{0, fishPersBuffer, sizeof(FishPer) * i, sizeof(FishPer)}});
        }
    }

    contextDawn->setBufferData(lightFactorBuffer, 0, sizeof(LightFactorUniforms),
                               &lightFactorUniforms);
    contextDawn->setBufferData(fishVertexBuffer, 0, sizeof(FishVertexUniforms),
                               &fishVertexUniforms);
}

void FishModelDawn::prepareForDraw() const {}

void FishModelDawn::draw()
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
    pass.SetIndexBuffer(indicesBuffer->getBuffer(), 0);

    if (enableDynamicBufferOffset)
    {
        for (int i = 0; i < instance; i++)
        {
            uint64_t offset = 256u * i;
            pass.SetBindGroup(3, bindGroupPers[0], 1, &offset);
            pass.DrawIndexed(indicesBuffer->getTotalComponents(), 1, 0, 0, 0);
        }
    }
    else
    {
        for (int i = 0; i < instance; i++)
        {
            pass.SetBindGroup(3, bindGroupPers[i], 0, nullptr);
            pass.DrawIndexed(indicesBuffer->getTotalComponents(), 1, 0, 0, 0);
        }
    }
}

void FishModelDawn::updatePerInstanceUniforms(const WorldUniforms &worldUniforms) {}

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
    fishPers[index].worldPosition[0] = x;
    fishPers[index].worldPosition[1] = y;
    fishPers[index].worldPosition[2] = z;
    fishPers[index].nextPosition[0]  = nextX;
    fishPers[index].nextPosition[1]  = nextY;
    fishPers[index].nextPosition[2]  = nextZ;
    fishPers[index].scale            = scale;
    fishPers[index].time             = time;
}

FishModelDawn::~FishModelDawn()
{
    pipeline          = nullptr;
    groupLayoutModel  = nullptr;
    groupLayoutPer    = nullptr;
    pipelineLayout    = nullptr;
    bindGroupModel    = nullptr;
    fishVertexBuffer  = nullptr;
    lightFactorBuffer = nullptr;
    fishPersBuffer    = nullptr;
    delete fishPers;
    if (enableDynamicBufferOffset)
    {
        bindGroupPers[0] = nullptr;
    }
    else
    {
        for (int i = 0; i < instance; i++)
        {
            bindGroupPers[i] = nullptr;
        }
    }

    bindGroupPers = nullptr;
}
