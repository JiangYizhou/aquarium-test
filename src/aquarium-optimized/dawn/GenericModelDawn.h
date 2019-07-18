//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// GenericModelDawn.h: Defnes generic model of Dawn

#pragma once
#ifndef GENERICMODELDAWN_H
#define GENERICMODELDAWN_H 1

#include "ContextDawn.h"
#include "ProgramDawn.h"
#include "dawn/dawncpp.h"
#include "utils/ComboRenderPipelineDescriptor.h"

#include "../Model.h"

class GenericModelDawn : public Model
{
  public:
    GenericModelDawn(const Context *context,
                     Aquarium *aquarium,
                     MODELGROUP type,
                     MODELNAME name,
                     bool blend);
    ~GenericModelDawn();

    void init() override;
    void prepareForDraw() const override;
    void draw() override;

    void updatePerInstanceUniforms(WorldUniforms *worldUniforms) override;

    TextureDawn *diffuseTexture;
    TextureDawn *normalTexture;
    TextureDawn *reflectionTexture;
    TextureDawn *skyboxTexture;

    BufferDawn *positionBuffer;
    BufferDawn *normalBuffer;
    BufferDawn *texCoordBuffer;
    BufferDawn *tangentBuffer;
    BufferDawn *binormalBuffer;

    BufferDawn *indicesBuffer;

    struct LightFactorUniforms
    {
        float shininess;
        float specularFactor;
    } lightFactorUniforms;

    struct WorldUniformPer
    {
        WorldUniforms WorldUniforms[20];
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
  dawn::Buffer worldBuffer;

  const ContextDawn *contextDawn;
  ProgramDawn *programDawn;

  int instance;
};

#endif