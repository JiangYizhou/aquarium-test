//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#include "OutsideModelDawn.h"

OutsideModelDawn::OutsideModelDawn(const Context *context,
                                   Aquarium *aquarium,
                                   MODELGROUP type,
                                   MODELNAME name,
                                   bool blend)
    : Model(type, name, blend)
{
    contextDawn = static_cast<const ContextDawn*>(context);

    lightFactorUniforms.shininess = 50.0f;
    lightFactorUniforms.specularFactor = 0.0f;
}

OutsideModelDawn::~OutsideModelDawn()
{
    pipeline          = nullptr;
    groupLayoutModel  = nullptr;
    groupLayoutPer    = nullptr;
    pipelineLayout    = nullptr;
    bindGroupModel    = nullptr;
    bindGroupPer      = nullptr;
    lightFactorBuffer = nullptr;
    viewBuffer        = nullptr;
}

void OutsideModelDawn::init()
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

    groupLayoutPer = contextDawn->MakeBindGroupLayout({
        {0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
    });

    // Outside models use diffuse shaders.
    groupLayoutModel = contextDawn->MakeBindGroupLayout({
        { 0, dawn::ShaderStageBit::Fragment, dawn::BindingType::UniformBuffer },
        { 1, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler },
        { 2, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture },
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
        dawn::BufferUsageBit::CopyDst | dawn::BufferUsageBit::Uniform);
    viewBuffer = contextDawn->createBufferFromData(
        &worldUniformPer, sizeof(WorldUniforms),
        dawn::BufferUsageBit::CopyDst | dawn::BufferUsageBit::Uniform);

    bindGroupModel = contextDawn->makeBindGroup(groupLayoutModel, {
        { 0, lightFactorBuffer, 0, sizeof(LightFactorUniforms) },
        { 1, diffuseTexture->getSampler() },
        { 2, diffuseTexture->getTextureView() },
    });

    bindGroupPer = contextDawn->makeBindGroup(groupLayoutPer, {
        {0, viewBuffer, 0, sizeof(WorldUniforms)},
    });

    contextDawn->setBufferData(lightFactorBuffer, 0, sizeof(LightFactorUniforms), &lightFactorUniforms);
}

void OutsideModelDawn::preDraw() const
{
}

void OutsideModelDawn::draw()
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
    if (tangentBuffer && binormalBuffer) {
        pass.SetVertexBuffers(3, 1, &tangentBuffer->getBuffer(), vertexBufferOffsets);
        pass.SetVertexBuffers(4, 1, &binormalBuffer->getBuffer(), vertexBufferOffsets);
    }
    pass.SetIndexBuffer(indicesBuffer->getBuffer(), 0);
    pass.DrawIndexed(indicesBuffer->getTotalComponents(), 1, 0, 0, 0);
}

void OutsideModelDawn::updatePerInstanceUniforms(WorldUniforms *worldUniforms) {
    memcpy(&worldUniformPer, worldUniforms, sizeof(WorldUniforms));

    contextDawn->setBufferData(viewBuffer, 0, sizeof(WorldUniforms), &worldUniformPer);
}
