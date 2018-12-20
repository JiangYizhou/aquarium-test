// GenericModelDawn.h: Defnes generic model of Dawn

#pragma once
#ifndef GENERICMODELDAWN_H
#define GENERICMODELDAWN_H 1

#include <string>

#include "../GenericModel.h"
#include "ContextDawn.h"
#include "ProgramDawn.h"
#include "dawn/dawncpp.h"

class GenericModelDawn : public GenericModel
{
public:
    GenericModelDawn(const Context* context, Aquarium* aquarium, MODELGROUP type, MODELNAME name, bool blend);

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