//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#include "OutsideModelDawn.h"

OutsideModelDawn::OutsideModelDawn(const Context* context, Aquarium* aquarium, MODELGROUP type, MODELNAME name, bool blend)
    : OutsideModel(type, name, blend)
{
    contextDawn = static_cast<const ContextDawn*>(context);

    lightFactorUniforms.shininess = 50.0f;
    lightFactorUniforms.specularFactor = 0.0f;
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

    inputState = contextDawn->createInputState({
        { 0, 0, dawn::VertexFormat::FloatR32G32B32, 0 },
        { 1, 1, dawn::VertexFormat::FloatR32G32B32, 0 },
        { 2, 2, dawn::VertexFormat::FloatR32G32, 0 },
        { 3, 3, dawn::VertexFormat::FloatR32G32B32, 0 },
        { 4, 4, dawn::VertexFormat::FloatR32G32B32, 0 },
    }, {
        { 0, positionBuffer->getDataSize(), dawn::InputStepMode::Vertex },
        { 1, normalBuffer->getDataSize(), dawn::InputStepMode::Vertex },
        { 2, texCoordBuffer->getDataSize(), dawn::InputStepMode::Vertex },
        { 3, tangentBuffer->getDataSize(), dawn::InputStepMode::Vertex },
        { 4, binormalBuffer->getDataSize(), dawn::InputStepMode::Vertex }
    });

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

    pipeline = contextDawn->createRenderPipeline(pipelineLayout, programDawn, inputState, mBlend);

    lightFactorBuffer = contextDawn->createBufferFromData(
        &lightFactorUniforms, sizeof(lightFactorUniforms),
        dawn::BufferUsageBit::TransferDst | dawn::BufferUsageBit::Uniform);
    viewBuffer = contextDawn->createBufferFromData(
        &viewUniformPer, sizeof(ViewUniforms),
        dawn::BufferUsageBit::TransferDst | dawn::BufferUsageBit::Uniform);

    bindGroupModel = contextDawn->makeBindGroup(groupLayoutModel, {
        { 0, lightFactorBuffer, 0, sizeof(LightFactorUniforms) },
        { 1, diffuseTexture->getSampler() },
        { 2, diffuseTexture->getTextureView() },
    });

    bindGroupPer = contextDawn->makeBindGroup(groupLayoutPer, {
        {0, viewBuffer, 0, sizeof(ViewUniforms)},
    });

    contextDawn->setBufferData(lightFactorBuffer, 0, sizeof(LightFactorUniforms), &lightFactorUniforms);
}

void OutsideModelDawn::preDraw() const
{
}

void OutsideModelDawn::draw()
{
    uint32_t vertexBufferOffsets[1] = { 0 };

    dawn::RenderPassEncoder pass = contextDawn->pass;
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

void OutsideModelDawn::updatePerInstanceUniforms(ViewUniforms *viewUniforms) {
    memcpy(&viewUniformPer, viewUniforms, sizeof(ViewUniforms));

    contextDawn->setBufferData(viewBuffer, 0, sizeof(ViewUniforms), &viewUniformPer);
}
