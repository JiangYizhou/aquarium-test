//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// InnerModelGL.cpp: Implement inner model of OpenGL.

#include "InnerModelGL.h"

InnerModelGL::InnerModelGL(const ContextGL *context,
                           Aquarium *aquarium,
                           MODELGROUP type,
                           MODELNAME name,
                           bool blend)
    : InnerModel(type, name, blend), contextGL(context)
{
    viewInverseUniform.first = aquarium->viewUniforms.viewInverse;
    lightWorldPosUniform.first = aquarium->lightWorldPositionUniform.lightWorldPos;

    worldUniform.first = aquarium->viewUniforms.world;
    worldViewProjectionUniform.first = aquarium->viewUniforms.worldViewProjection;
    worldInverseTransposeUniform.first = aquarium->viewUniforms.worldInverseTranspose;

    etaUniform.first = 1.0f;
    tankColorFudgeUniform.first = 0.796f;
    refractionFudgeUniform.first = 3.0f;

    fogPowerUniform.first = g_fogPower;
    fogMultUniform.first = g_fogMult;
    fogOffsetUniform.first = g_fogOffset;
    fogColorUniform.first = aquarium->fogUniforms.fogColor;
}

void InnerModelGL::init()
{
    ProgramGL *programGL = static_cast<ProgramGL *>(mProgram);
    worldViewProjectionUniform.second =
        contextGL->getUniformLocation(programGL->getProgramId(), "worldViewProjection");
    worldUniform.second = contextGL->getUniformLocation(programGL->getProgramId(), "world");
    worldInverseTransposeUniform.second =
        contextGL->getUniformLocation(programGL->getProgramId(), "worldInverseTranspose");

    viewInverseUniform.second =
        contextGL->getUniformLocation(programGL->getProgramId(), "viewInverse");
    lightWorldPosUniform.second =
        contextGL->getUniformLocation(programGL->getProgramId(), "lightWorldPos");

    fogPowerUniform.second  = contextGL->getUniformLocation(programGL->getProgramId(), "fogPower");
    fogMultUniform.second   = contextGL->getUniformLocation(programGL->getProgramId(), "fogMult");
    fogOffsetUniform.second = contextGL->getUniformLocation(programGL->getProgramId(), "fogOffset");
    fogColorUniform.second  = contextGL->getUniformLocation(programGL->getProgramId(), "fogColor");

    etaUniform.second = contextGL->getUniformLocation(programGL->getProgramId(), "eta");
    tankColorFudgeUniform.second =
        contextGL->getUniformLocation(programGL->getProgramId(), "tankColorFudge");
    refractionFudgeUniform.second =
        contextGL->getUniformLocation(programGL->getProgramId(), "refractionFudge");

    diffuseTexture.first    = static_cast<TextureGL *>(textureMap["diffuse"]);
    diffuseTexture.second   = contextGL->getUniformLocation(programGL->getProgramId(), "diffuse");
    normalTexture.first     = static_cast<TextureGL *>(textureMap["normalMap"]);
    normalTexture.second    = contextGL->getUniformLocation(programGL->getProgramId(), "normalMap");
    reflectionTexture.first = static_cast<TextureGL *>(textureMap["reflectionMap"]);
    reflectionTexture.second =
        contextGL->getUniformLocation(programGL->getProgramId(), "reflectionMap");
    skyboxTexture.first  = static_cast<TextureGL *>(textureMap["skybox"]);
    skyboxTexture.second = contextGL->getUniformLocation(programGL->getProgramId(), "skybox");

    positionBuffer.first  = static_cast<BufferGL *>(bufferMap["position"]);
    positionBuffer.second = contextGL->getAttribLocation(programGL->getProgramId(), "position");
    normalBuffer.first    = static_cast<BufferGL *>(bufferMap["normal"]);
    normalBuffer.second   = contextGL->getAttribLocation(programGL->getProgramId(), "normal");
    texCoordBuffer.first  = static_cast<BufferGL *>(bufferMap["texCoord"]);
    texCoordBuffer.second = contextGL->getAttribLocation(programGL->getProgramId(), "texCoord");
    tangentBuffer.first   = static_cast<BufferGL *>(bufferMap["tangent"]);
    tangentBuffer.second  = contextGL->getAttribLocation(programGL->getProgramId(), "tangent");
    binormalBuffer.first  = static_cast<BufferGL *>(bufferMap["binormal"]);
    binormalBuffer.second = contextGL->getAttribLocation(programGL->getProgramId(), "binormal");

    indicesBuffer = static_cast<BufferGL *>(bufferMap["indices"]);
}

void InnerModelGL::draw()
{
    contextGL->drawElements(indicesBuffer);
}

void InnerModelGL::preDraw() const
{
    mProgram->setProgram();
    contextGL->enableBlend(mBlend);

    ProgramGL *programGL = static_cast<ProgramGL *>(mProgram);
    contextGL->bindVAO(programGL->getVAOId());

    contextGL->setAttribs(positionBuffer.first, positionBuffer.second);
    contextGL->setAttribs(normalBuffer.first, normalBuffer.second);
    contextGL->setAttribs(texCoordBuffer.first, texCoordBuffer.second);

    contextGL->setAttribs(tangentBuffer.first, tangentBuffer.second);
    contextGL->setAttribs(binormalBuffer.first, binormalBuffer.second);

    contextGL->setIndices(indicesBuffer);

    contextGL->setUniform(viewInverseUniform.second, viewInverseUniform.first, GL_FLOAT_MAT4);
    contextGL->setUniform(lightWorldPosUniform.second, lightWorldPosUniform.first, GL_FLOAT_VEC3);
    contextGL->setUniform(fogPowerUniform.second, &fogPowerUniform.first, GL_FLOAT);
    contextGL->setUniform(fogMultUniform.second, &fogMultUniform.first, GL_FLOAT);
    contextGL->setUniform(fogOffsetUniform.second, &fogOffsetUniform.first, GL_FLOAT);
    contextGL->setUniform(fogColorUniform.second, fogColorUniform.first, GL_FLOAT_VEC4);
    contextGL->setUniform(etaUniform.second, &etaUniform.first, GL_FLOAT);
    contextGL->setUniform(tankColorFudgeUniform.second, &tankColorFudgeUniform.first, GL_FLOAT);
    contextGL->setUniform(refractionFudgeUniform.second, &refractionFudgeUniform.first, GL_FLOAT);

    contextGL->setTexture(diffuseTexture.first, diffuseTexture.second, 0);
    contextGL->setTexture(normalTexture.first, normalTexture.second, 1);
    contextGL->setTexture(reflectionTexture.first, reflectionTexture.second, 2);
    contextGL->setTexture(skyboxTexture.first, skyboxTexture.second, 3);
}

void InnerModelGL::updatePerInstanceUniforms(ViewUniforms* viewUniforms)
{
    contextGL->setUniform(worldUniform.second, worldUniform.first, GL_FLOAT_MAT4);
    contextGL->setUniform(worldViewProjectionUniform.second, worldViewProjectionUniform.first,
                          GL_FLOAT_MAT4);
    contextGL->setUniform(worldInverseTransposeUniform.second, worldInverseTransposeUniform.first,
                          GL_FLOAT_MAT4);
}
