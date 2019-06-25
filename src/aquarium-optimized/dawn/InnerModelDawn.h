//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// InnerModelDawn.h: Defines inner model of Dawn.

#pragma once
#ifndef INNERMODELDAWN_H
#define INNERMODELDAWN_H 1

#include "../Model.h"
#include "ContextDawn.h"
#include "ProgramDawn.h"
#include "dawn/dawncpp.h"
#include "utils/ComboRenderPipelineDescriptor.h"

class InnerModelDawn : public Model
{
  public:
    InnerModelDawn(const Context* context, Aquarium* aquarium, MODELGROUP type, MODELNAME name, bool blend);
    ~InnerModelDawn();

    void init() override;
    void preDraw() const override;
    void draw() override;
    void updatePerInstanceUniforms(WorldUniforms *WorldUniforms) override;

    struct InnerUniforms
    {
        float eta;
        float tankColorFudge;
        float refractionFudge;
        float padding;
    } innerUniforms;

    WorldUniforms worldUniformPer;

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


private:
  utils::ComboVertexInputDescriptor vertexInputDescriptor;
  dawn::RenderPipeline pipeline;

  dawn::BindGroupLayout groupLayoutModel;
  dawn::BindGroupLayout groupLayoutPer;
  dawn::PipelineLayout pipelineLayout;

  dawn::BindGroup bindGroupModel;
  dawn::BindGroup bindGroupPer;

  dawn::Buffer innerBuffer;
  dawn::Buffer viewBuffer;

  const ContextDawn *contextDawn;
  ProgramDawn *programDawn;
};

#endif // !INNERMODELDAWN_H
#pragma once
