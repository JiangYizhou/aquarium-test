// FishModelDawn.cpp: Implements fish model of Dawn.

#include "FishModelDawn.h"
#include "BufferDawn.h"

FishModelDawn::FishModelDawn(const Context *context,
                             Aquarium *aquarium,
                             MODELGROUP type,
                             MODELNAME name,
                             bool blend)
    : FishModel(type, name, blend)
{
    contextDawn = static_cast<const ContextDawn *>(context);

    lightFactorUniforms.shininess      = 5.0f;
    lightFactorUniforms.specularFactor = 0.3f;

    fishPerUniforms.scale = 1;
}

void FishModelDawn::init()
{
    programDawn = static_cast<ProgramDawn *>(mProgram);

    diffuseTexture    = static_cast<TextureDawn *>(textureMap["diffuse"]);
    normalTexture     = static_cast<TextureDawn *>(textureMap["normalMap"]);
    reflectionTexture = static_cast<TextureDawn *>(textureMap["reflectionMap"]);
    skyboxTexture     = static_cast<TextureDawn *>(textureMap["skybox"]);

    positionBuffer = static_cast<BufferDawn *>(bufferMap["position"]);
    normalBuffer   = static_cast<BufferDawn *>(bufferMap["normal"]);
    texCoordBuffer = static_cast<BufferDawn *>(bufferMap["texCoord"]);
    tangentBuffer  = static_cast<BufferDawn *>(bufferMap["tangent"]);
    binormalBuffer = static_cast<BufferDawn *>(bufferMap["binormal"]);
    indicesBuffer  = static_cast<BufferDawn *>(bufferMap["indices"]);

    inputState = contextDawn->createInputState(
        {
            {0, 0, dawn::VertexFormat::FloatR32G32B32, 0},
            {1, 1, dawn::VertexFormat::FloatR32G32B32, 0},
            {2, 2, dawn::VertexFormat::FloatR32G32, 0},
            {3, 3, dawn::VertexFormat::FloatR32G32B32, 0},
            {4, 4, dawn::VertexFormat::FloatR32G32B32, 0},
        },
        {{0, positionBuffer->getDataSize(), dawn::InputStepMode::Vertex},
         {1, normalBuffer->getDataSize(), dawn::InputStepMode::Vertex},
         {2, texCoordBuffer->getDataSize(), dawn::InputStepMode::Vertex},
         {3, tangentBuffer->getDataSize(), dawn::InputStepMode::Vertex},
         {4, binormalBuffer->getDataSize(), dawn::InputStepMode::Vertex}});

    // TODO(yizhou) : Check if the layout works for normal map fragment shader
    // becasue the shader doesn't contains reflaction map and skybox
    if (skyboxTexture && reflectionTexture)
    {
        groupLayoutModel = contextDawn->MakeBindGroupLayout({
            {0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
            {1, dawn::ShaderStageBit::Fragment, dawn::BindingType::UniformBuffer},
            {2, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler},
            {3, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler},
            {4, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
            {5, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
            {6, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
            {7, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
        });
    }
    else
    {
        groupLayoutModel = contextDawn->MakeBindGroupLayout({
            {0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
            {1, dawn::ShaderStageBit::Fragment, dawn::BindingType::UniformBuffer},
            {2, dawn::ShaderStageBit::Fragment, dawn::BindingType::Sampler},
            {3, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
            {4, dawn::ShaderStageBit::Fragment, dawn::BindingType::SampledTexture},
        });
    }

    groupLayoutPer = contextDawn->MakeBindGroupLayout({
        {0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
        {1, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer},
    });

    pipelineLayout = contextDawn->MakeBasicPipelineLayout({
        contextDawn->groupLayoutGeneral,
        contextDawn->groupLayoutWorld,
        groupLayoutModel,
        groupLayoutPer,
    });

    pipeline = contextDawn->createRenderPipeline(pipelineLayout, programDawn, inputState, mBlend);

    fishVertexBuffer = contextDawn->createBufferFromData(
        &fishVertexUniforms, sizeof(FishVertexUniforms),
        dawn::BufferUsageBit::TransferDst | dawn::BufferUsageBit::Uniform);
    lightFactorBuffer = contextDawn->createBufferFromData(
        &lightFactorUniforms, sizeof(LightFactorUniforms),
        dawn::BufferUsageBit::TransferDst | dawn::BufferUsageBit::Uniform);
    fishPerBuffer = contextDawn->createBuffer(
        sizeof(FishPerUniforms), dawn::BufferUsageBit::TransferDst | dawn::BufferUsageBit::Uniform);
    viewBuffer = contextDawn->createBufferFromData(
        &viewUniformPer, sizeof(ViewUniforms),
        dawn::BufferUsageBit::TransferDst | dawn::BufferUsageBit::Uniform);

    // Fish models includes small, medium and big. Some of them contains reflection and skybox
    // texture, but some doesn't.
    if (skyboxTexture && reflectionTexture)
    {
        bindGroupModel = contextDawn->makeBindGroup(
            groupLayoutModel, {{0, fishVertexBuffer, 0, sizeof(FishVertexUniforms)},
                               {1, lightFactorBuffer, 0, sizeof(LightFactorUniforms)},
                               {2, reflectionTexture->getSampler()},
                               {3, skyboxTexture->getSampler()},
                               {4, diffuseTexture->getTextureView()},
                               {5, normalTexture->getTextureView()},
                               {6, reflectionTexture->getTextureView()},
                               {7, skyboxTexture->getTextureView()}});
    }
    else
    {
        bindGroupModel = contextDawn->makeBindGroup(
            groupLayoutModel, {{0, fishVertexBuffer, 0, sizeof(FishVertexUniforms)},
                               {1, lightFactorBuffer, 0, sizeof(LightFactorUniforms)},
                               {2, diffuseTexture->getSampler()},
                               {3, diffuseTexture->getTextureView()},
                               {4, normalTexture->getTextureView()}});
    }

    bindGroupPer = contextDawn->makeBindGroup(groupLayoutPer,
                                              {
                                                  {0, viewBuffer, 0, sizeof(ViewUniforms)},
                                                  {1, fishPerBuffer, 0, sizeof(FishPerUniforms)},
                                              });

    contextDawn->setBufferData(lightFactorBuffer, 0, sizeof(LightFactorUniforms),
                               &lightFactorUniforms);
}

void FishModelDawn::applyUniforms() const
{
    contextDawn->setBufferData(fishVertexBuffer, 0, sizeof(FishVertexUniforms),
                               &fishVertexUniforms);
}

void FishModelDawn::applyTextures() const
{
    // applied during init
}

void FishModelDawn::applyBuffers() const
{
    // applied befor draw, apply in renderpass
}

void FishModelDawn::draw()
{
    uint32_t vertexBufferOffsets[1] = {0};

    dawn::RenderPassEncoder pass = contextDawn->pass;
    pass.SetPipeline(pipeline);
    pass.SetBindGroup(0, contextDawn->bindGroupGeneral);
    pass.SetBindGroup(1, contextDawn->bindGroupWorld);
    pass.SetBindGroup(2, bindGroupModel);
    pass.SetBindGroup(3, bindGroupPer);
    pass.SetVertexBuffers(0, 1, &positionBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(1, 1, &normalBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(2, 1, &texCoordBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(3, 1, &tangentBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetVertexBuffers(4, 1, &binormalBuffer->getBuffer(), vertexBufferOffsets);
    pass.SetIndexBuffer(indicesBuffer->getBuffer(), 0);
    pass.DrawIndexed(indicesBuffer->getTotalComponents(), 1, 0, 0, 0);
}

void FishModelDawn::updatePerInstanceUniforms(ViewUniforms *viewUniforms)
{
    memcpy(&viewUniformPer, viewUniforms, sizeof(ViewUniforms));

    contextDawn->setBufferData(viewBuffer, 0, sizeof(ViewUniforms), &viewUniformPer);
    contextDawn->setBufferData(fishPerBuffer, 0, sizeof(FishPerUniforms), &fishPerUniforms);
}

void FishModelDawn::updateFishCommonUniforms(float fishLength,
                                             float fishBendAmount,
                                             float fishWaveLength)
{
    fishVertexUniforms.fishLength     = fishLength;
    fishVertexUniforms.fishBendAmount = fishBendAmount;
    fishVertexUniforms.fishWaveLength = fishWaveLength;
}

void FishModelDawn::updateFishPerUniforms(float x,
                                          float y,
                                          float z,
                                          float nextX,
                                          float nextY,
                                          float nextZ,
                                          float scale,
                                          float time)
{
    fishPerUniforms.worldPosition[0] = x;
    fishPerUniforms.worldPosition[1] = y;
    fishPerUniforms.worldPosition[2] = z;
    fishPerUniforms.nextPosition[0]  = nextX;
    fishPerUniforms.nextPosition[1]  = nextY;
    fishPerUniforms.nextPosition[2]  = nextZ;
    fishPerUniforms.scale            = scale;
    fishPerUniforms.time             = time;
}
