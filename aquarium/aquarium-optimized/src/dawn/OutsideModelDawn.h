//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// OutsideModelDawn.h: Defnes outside model of Dawn

#pragma once
#ifndef OUTSIDEMODELDAWN_H
#define OUTSIDEMODELDAWN_H 1

#include <string>

#include "../OutsideModel.h"
#include "ContextDawn.h"
#include "ProgramDawn.h"
#include "dawn/dawncpp.h"

class OutsideModelDawn : public OutsideModel
{
public:
    OutsideModelDawn(const Context* context, Aquarium* aquarium, MODELGROUP type, MODELNAME name, bool blend);

    void init() override;
    void preDraw() const override;
    void draw() override;

    void updatePerInstanceUniforms(ViewUniforms *viewUniforms) override;

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

    ViewUniforms viewUniformPer;

private:
    dawn::InputState inputState;
    dawn::RenderPipeline pipeline;

    dawn::BindGroupLayout groupLayoutModel;
    dawn::BindGroupLayout groupLayoutPer;
    dawn::PipelineLayout pipelineLayout;

    dawn::BindGroup bindGroupModel;
    dawn::BindGroup bindGroupPer;

    dawn::Buffer lightFactorBuffer;
    dawn::Buffer viewBuffer;

    const ContextDawn *contextDawn;
    ProgramDawn* programDawn;
};

#endif