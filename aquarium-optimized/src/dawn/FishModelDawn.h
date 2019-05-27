//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelDawn.h: Defnes fish model of Dawn

#pragma once
#ifndef FISHMODELDAWN_H
#define FISHMODELDAWN_H 1

#include <string>

#include "../FishModel.h"
#include "ContextDawn.h"
#include "ProgramDawn.h"
#include "dawn/dawncpp.h"

class FishModelDawn : public FishModel
{
  public:
    FishModelDawn(const Context *context,
                  Aquarium *aquarium,
                  MODELGROUP type,
                  MODELNAME name,
                  bool blend);
    ~FishModelDawn();

    void init() override;
    void preDraw() const override;
    void draw() override;

    void updatePerInstanceUniforms(WorldUniforms *worldUniforms) override;
    void updateFishPerUniforms(float x,
                               float y,
                               float z,
                               float nextX,
                               float nextY,
                               float nextZ,
                               float scale,
                               float time,
                               int index) override;

    struct FishVertexUniforms
    {
        float fishLength;
        float fishWaveLength;
        float fishBendAmount;
    } fishVertexUniforms;

    struct LightFactorUniforms
    {
        float shininess;
        float specularFactor;
    } lightFactorUniforms;

    struct FishPer
    {
        float worldPosition[3];
        float scale;
        float nextPosition[3];
        float time;
        float padding[56];  // TODO(yizhou): the padding is to align with 256 byte offset.
    };
    FishPer *fishPers;

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
    dawn::VertexInputDescriptor vertexInputDescriptor;
    dawn::RenderPipeline pipeline;

    dawn::BindGroupLayout groupLayoutModel;
    dawn::BindGroupLayout groupLayoutPer;
    dawn::PipelineLayout pipelineLayout;

    dawn::BindGroup bindGroupModel;
    dawn::BindGroup *bindGroupPers;

    dawn::Buffer fishVertexBuffer;
    dawn::Buffer lightFactorBuffer;

    dawn::Buffer fishPersBuffer;

    int instance;

    ProgramDawn *programDawn;
    const ContextDawn *contextDawn;
};

#endif