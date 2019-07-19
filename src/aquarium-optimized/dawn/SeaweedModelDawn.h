//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// SeaweedModelDawn.h: Defines seaweed model of Dawn.

#pragma once
#ifndef SEAWEEDMODELDAWN_H
#define SEAWEEDMODELDAWN_H 1

#include "ContextDawn.h"
#include "ProgramDawn.h"
#include "dawn/dawncpp.h"
#include "utils/ComboRenderPipelineDescriptor.h"

#include "../SeaweedModel.h"

class SeaweedModelDawn : public SeaweedModel
{
  public:
    SeaweedModelDawn(const Context *context, Aquarium *aquarium, MODELGROUP type, MODELNAME name, bool blend);
    ~SeaweedModelDawn();

    void init() override;
    void prepareForDraw() const override;
    void draw() override;

    void updatePerInstanceUniforms(const WorldUniforms &worldUniforms) override;


    TextureDawn *diffuseTexture;
    TextureDawn *normalTexture;
    TextureDawn *reflectionTexture;
    TextureDawn *skyboxTexture;

    BufferDawn *positionBuffer;
    BufferDawn *normalBuffer;
    BufferDawn *texCoordBuffer;

    BufferDawn *indicesBuffer;
    void updateSeaweedModelTime(float time) override;

    struct LightFactorUniforms
    {
        float shininess;
        float specularFactor;
    } lightFactorUniforms;

    struct SeaweedPer
    {
        float time[20];
    } seaweedPer;

    struct WorldUniformPer
    {
        WorldUniforms worldUniforms[20];
    };
    WorldUniformPer worldUniformPer;

  private:
    utils::ComboVertexInputDescriptor vertexInputDescriptor;
    dawn::RenderPipeline pipeline;

    dawn::BindGroupLayout groupLayoutModel;
    dawn::BindGroupLayout groupLayoutPer;
    dawn::PipelineLayout pipelineLayout;

    dawn::BindGroup bindGroupModel;
    dawn::BindGroup bindGroupPer;

    dawn::Buffer lightFactorBuffer;
    dawn::Buffer timeBuffer;
    dawn::Buffer viewBuffer;

    const ContextDawn *contextDawn;
    ProgramDawn* programDawn;
    Aquarium * mAquarium;

    int instance;
};

#endif // !SEAWEEDMODEL_H