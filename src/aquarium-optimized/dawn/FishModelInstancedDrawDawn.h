//
// Copyright (c) 2019 The Native Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelDawn.h: Defnes fish model of Dawn

#pragma once
#ifndef FISHMODELINSTANCEDDRAWDAWN_H
#define FISHMODELINSTANCEDDRAWDAWN_H 1

#include "ContextDawn.h"
#include "ProgramDawn.h"
#include "dawn/dawncpp.h"
#include "utils/ComboRenderPipelineDescriptor.h"

#include "../FishModel.h"

class FishModelInstancedDrawDawn : public FishModel
{
  public:
    FishModelInstancedDrawDawn(const Context *context,
                               Aquarium *aquarium,
                               MODELGROUP type,
                               MODELNAME name,
                               bool blend);
    ~FishModelInstancedDrawDawn();

    void init() override;
    void prepareForDraw() const override;
    void draw() override;

    void updatePerInstanceUniforms(const WorldUniforms &worldUniforms) override;
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
    } mFishVertexUniforms;

    struct LightFactorUniforms
    {
        float shininess;
        float specularFactor;
    } mLightFactorUniforms;

    struct FishPer
    {
        float worldPosition[3];
        float scale;
        float nextPosition[3];
        float time;
    };
    FishPer *mFishPers;

    TextureDawn *mDiffuseTexture;
    TextureDawn *mNormalTexture;
    TextureDawn *mReflectionTexture;
    TextureDawn *mSkyboxTexture;

    BufferDawn *mPositionBuffer;
    BufferDawn *mNormalBuffer;
    BufferDawn *mTexCoordBuffer;
    BufferDawn *mTangentBuffer;
    BufferDawn *mBiNormalBuffer;

    BufferDawn *mIndicesBuffer;

  private:
    utils::ComboVertexInputDescriptor mVertexInputDescriptor;
    dawn::RenderPipeline mPipeline;

    dawn::BindGroupLayout mGroupLayoutModel;
    dawn::BindGroupLayout mGroupLayoutPer;
    dawn::PipelineLayout mPipelineLayout;

    dawn::BindGroup mBindGroupModel;
    dawn::BindGroup mBindGroupPer;

    dawn::Buffer mFishVertexBuffer;
    dawn::Buffer mLightFactorBuffer;

    dawn::Buffer mFishPersBuffer;

    int instance;

    ProgramDawn *mProgramDawn;
    const ContextDawn *mContextDawn;
};

#endif