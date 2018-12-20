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
    void applyUniforms() const override;
    void applyTextures() const override;
    void applyBuffers() const override;
    void draw() const override;

    void updatePerInstanceUniforms() const override;


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

private:
    dawn::InputState inputState;
    dawn::RenderPipeline pipeline;

    dawn::BindGroupLayout groupLayoutModel;
    dawn::PipelineLayout pipelineLayout;

    dawn::BindGroup bindGroupModel;

    dawn::Buffer lightFactorBuffer;

    const ContextDawn *contextDawn;
    ProgramDawn* programDawn;
};

#endif