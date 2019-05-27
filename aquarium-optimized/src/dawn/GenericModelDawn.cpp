//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#include "GenericModelDawn.h"

#include "../Aquarium.h"

GenericModelDawn::GenericModelDawn(const Context *context,
                                   Aquarium *aquarium,
                                   MODELGROUP type,
                                   MODELNAME name,
                                   bool blend)
    : GenericModel(type, name, blend), instance(0)
{
    contextDawn = static_cast<const ContextDawn *>(context);

    lightFactorUniforms.shininess      = 50.0f;
    lightFactorUniforms.specularFactor = 1.0f;
}

GenericModelDawn::~GenericModelDawn()
{
    pipeline          = nullptr;
    groupLayoutModel  = nullptr;
    groupLayoutPer    = nullptr;
    pipelineLayout    = nullptr;
    bindGroupModel    = nullptr;
    bindGroupPer      = nullptr;
    lightFactorBuffer = nullptr;
    worldBuffer       = nullptr;
}

void GenericModelDawn::init()
{
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

    // Generic models use reflection, normal or diffuse shaders, of which groupLayouts are
    // diiferent in texture binding.  MODELGLOBEBASE use diffuse shader though it contains
    // normal and reflection textures.
    std::vector<dawn::VertexAttributeDescriptor> vertexAttributeDescriptor;
    std::vector<dawn::VertexBufferDescriptor> vertexBufferDescriptor;
    if (normalTexture && mName != MODELNAME::MODELGLOBEBASE)
    {
        contextDawn->createInputState(
            &vertexInputDescriptor, vertexAttributeDescriptor, vertexBufferDescriptor,
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
    }
    else
    {
        contextDawn->createInputState(
            &vertexInputDescriptor, vertexAttributeDescriptor, vertexBufferDescriptor,
            {
                {0, 0, dawn::VertexFormat::Float3, 0},
                {1, 1, dawn::VertexFormat::Float3, 0},
                {2, 2, dawn::VertexFormat::Float2, 0},
            },
            {
                {0, positionBuffer->getDataSize(), dawn::InputStepMode::Vertex},
                {1, normalBuffer->getDataSize(), dawn::InputStepMode::Vertex},
                {2, texCoordBuffer->getDataSize(), dawn::InputStepMode::Vertex},
            });
    }

    if (skyboxTexture && reflectionTexture && mName != MODELNAME::MODELGLOBEBASE)
    {
        groupLayoutModel = contextDawn->MakeBindGroupLayout({
            {0, dawn::ShaderStageBit::Fragment, dawn::BindingType::UniformBuffer},
            {1, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler},
            {2, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler},
            {3, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
            {4, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
            {5, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
            {6, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
        });
    }
    else if (normalTexture && mName != MODELNAME::MODELGLOBEBASE)
    {
        groupLayoutModel = contextDawn->MakeBindGroupLayout({
            {0, dawn::ShaderStageBit::Fragment, dawn::BindingType::UniformBuffer},
            {1, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler},
            {2, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
            {3, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
        });
    }
    else
    {
        groupLayoutModel = contextDawn->MakeBindGroupLayout({
            {0, dawn::ShaderStageBit::Fragment, dawn::BindingType::UniformBuffer},
            {1, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler},
            {2, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
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

    pipeline = contextDawn->createRenderPipeline(pipelineLayout, programDawn, vertexInputDescriptor,
                                                 mBlend);

    lightFactorBuffer = contextDawn->createBufferFromData(
        &lightFactorUniforms, sizeof(lightFactorUniforms),
        dawn::BufferUsageBit::TransferDst | dawn::BufferUsageBit::Uniform);
    worldBuffer = contextDawn->createBufferFromData(
        &worldUniformPer, sizeof(worldUniformPer),
        dawn::BufferUsageBit::TransferDst | dawn::BufferUsageBit::Uniform);

    // Generic models use reflection, normal or diffuse shaders, of which grouplayouts are
    // diiferent in texture binding. MODELGLOBEBASE use diffuse shader though it contains
    // normal and reflection textures.
    if (skyboxTexture && reflectionTexture && mName != MODELNAME::MODELGLOBEBASE)
    {
        bindGroupModel = contextDawn->makeBindGroup(
            groupLayoutModel, {{0, lightFactorBuffer, 0, sizeof(LightFactorUniforms)},
                               {1, reflectionTexture->getSampler()},
                               {2, skyboxTexture->getSampler()},
                               {3, diffuseTexture->getTextureView()},
                               {4, normalTexture->getTextureView()},
                               {5, reflectionTexture->getTextureView()},
                               {6, skyboxTexture->getTextureView()}});
    }
    else if (normalTexture && mName != MODELNAME::MODELGLOBEBASE)
    {
        bindGroupModel = contextDawn->makeBindGroup(
            groupLayoutModel, {
                                  {0, lightFactorBuffer, 0, sizeof(LightFactorUniforms)},
                                  {1, diffuseTexture->getSampler()},
                                  {2, diffuseTexture->getTextureView()},
                                  {3, normalTexture->getTextureView()},
                              });
    }
    else
    {
        bindGroupModel = contextDawn->makeBindGroup(
            groupLayoutModel, {
                                  {0, lightFactorBuffer, 0, sizeof(LightFactorUniforms)},
                                  {1, diffuseTexture->getSampler()},
                                  {2, diffuseTexture->getTextureView()},
                              });
    }

    bindGroupPer =
        contextDawn->makeBindGroup(groupLayoutPer, {
                                                       {0, worldBuffer, 0, sizeof(WorldUniformPer)},
                                                   });

    contextDawn->setBufferData(lightFactorBuffer, 0, sizeof(LightFactorUniforms),
                               &lightFactorUniforms);
}

void GenericModelDawn::preDraw() const
{
    contextDawn->setBufferData(worldBuffer, 0, sizeof(WorldUniformPer), &worldUniformPer);
}

void GenericModelDawn::draw()
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
    // diffuseShader doesn't have to input tangent buffer or binormal buffer.
    if (tangentBuffer && binormalBuffer && mName != MODELNAME::MODELGLOBEBASE)
    {
        pass.SetVertexBuffers(3, 1, &tangentBuffer->getBuffer(), vertexBufferOffsets);
        pass.SetVertexBuffers(4, 1, &binormalBuffer->getBuffer(), vertexBufferOffsets);
    }
    pass.SetIndexBuffer(indicesBuffer->getBuffer(), 0);
    pass.DrawIndexed(indicesBuffer->getTotalComponents(), instance, 0, 0, 0);
    instance = 0;
}

void GenericModelDawn::updatePerInstanceUniforms(WorldUniforms *worldUniforms)
{
    worldUniformPer.WorldUniforms[instance] = *worldUniforms;

    instance++;
}
