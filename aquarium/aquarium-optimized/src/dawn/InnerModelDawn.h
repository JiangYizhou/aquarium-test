// InnerModelDawn.h: Defines inner model of Dawn.

#pragma once
#ifndef INNERMODELDAWN_H
#define INNERMODELDAWN_H 1

#include "../InnerModel.h"
#include "ContextDawn.h"
#include "ProgramDawn.h"
#include "dawn/dawncpp.h"

class InnerModelDawn : public InnerModel
{
  public:
    InnerModelDawn(const Context* context, Aquarium* aquarium, MODELGROUP type, MODELNAME name, bool blend);

    void init() override;
    void applyUniforms() const override;
    void applyTextures() const override;
    void applyBuffers() const override;
    void draw() override;
    void updatePerInstanceUniforms(ViewUniforms *viewUniforms) override;

    struct InnerUniforms
    {
        float eta;
        float tankColorFudge;
        float refractionFudge;
    } innerUniforms;

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

    dawn::Buffer innerBuffer;
    dawn::Buffer viewBuffer;

    const ContextDawn *contextDawn;
    ProgramDawn* programDawn;
};

#endif // !INNERMODELDAWN_H
#pragma once
