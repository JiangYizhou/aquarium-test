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
    FishModelDawn(const Context* context, Aquarium* aquarium, MODELGROUP type, MODELNAME name, bool blend);

    void init() override;
    void applyUniforms() const override;
    void applyTextures() const override;
    void applyBuffers() const override;
    void draw() override;

    void updatePerInstanceUniforms(ViewUniforms *viewUniforms) override;
    void updateFishCommonUniforms(float fishLength,
        float fishBendAmount,
        float fishWaveLength) override;
    void updateFishPerUniforms(float x,
        float y,
        float z,
        float nextX,
        float nextY,
        float nextZ,
        float scale,
        float time) override;

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

    struct FishPerUniforms
    {
        float worldPosition[3];
        float padding1;
        float nextPosition[3];
        float padding2;
        float scale;
        float time;
    } fishPerUniforms;

    ViewUniforms viewUniformPer;

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
    dawn::InputState inputState;
    dawn::RenderPipeline pipeline;

    dawn::BindGroupLayout groupLayoutModel;
    dawn::BindGroupLayout groupLayoutPer;
    dawn::PipelineLayout pipelineLayout;

    dawn::BindGroup bindGroupModel;
    dawn::BindGroup bindGroupPer;
    
    dawn::Buffer fishVertexBuffer;
    dawn::Buffer fishPerBuffer;
    dawn::Buffer lightFactorBuffer;
    dawn::Buffer viewBuffer;

    ProgramDawn* programDawn;
    const ContextDawn* contextDawn;
};

#endif