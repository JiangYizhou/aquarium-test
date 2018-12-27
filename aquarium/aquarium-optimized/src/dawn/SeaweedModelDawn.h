// SeaweedModelDawn.h: Defines seaweed model of Dawn.

#pragma once
#ifndef SEAWEEDMODELDAWN_H
#define SEAWEEDMODELDAWN_H 1

#include "../SeaweedModel.h"
#include "ContextDawn.h"
#include "ProgramDawn.h"
#include "dawn/dawncpp.h"

class SeaweedModelDawn : public SeaweedModel
{
  public:
    SeaweedModelDawn(const Context* context, Aquarium* aquarium, MODELGROUP type, MODELNAME name, bool blend);

    void init() override;
    void applyUniforms() const override;
    void applyTextures() const override;
    void applyBuffers() const override;
    void draw() override;

    void updatePerInstanceUniforms(ViewUniforms *viewUniforms) override;


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
        float time;
    } seaweedPer;

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
    dawn::Buffer timeBuffer;
    dawn::Buffer viewBuffer;

    const ContextDawn *contextDawn;
    ProgramDawn* programDawn;
};

#endif // !SEAWEEDMODEL_H