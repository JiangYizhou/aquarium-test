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

    struct ViewUniformPer
    {
        ViewUniforms viewuniforms[20];
    };
    ViewUniformPer viewUniformPer;

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

    int instance;
};

#endif