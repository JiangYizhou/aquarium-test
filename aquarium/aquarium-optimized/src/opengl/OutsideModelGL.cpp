//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// OutsideModelGL.cpp: Implement outside model of OpenGL.

#include "OutsideModelGL.h"

OutsideModelGL::OutsideModelGL(const ContextGL *context,
                               Aquarium* aquarium,
                               MODELGROUP type,
                               MODELNAME name,
                               bool blend)
    : OutsideModel(type, name, blend), contextGL(context)
{
    viewInverseUniform.first = aquarium->viewUniforms.viewInverse;
    lightWorldPosUniform.first = aquarium->lightWorldPositionUniform.lightWorldPos;
    lightColorUniform.first = aquarium->lightUniforms.lightColor;
    specularUniform.first = aquarium->lightUniforms.specular;
    shininessUniform.first = 50.0f;
    specularFactorUniform.first = 0.0f;
    ambientUniform.first = aquarium->lightUniforms.ambient;
    worldUniform.first = aquarium->viewUniforms.world;
    worldViewProjectionUniform.first = aquarium->viewUniforms.worldViewProjection;
    worldInverseTransposeUniform.first = aquarium->viewUniforms.worldInverseTranspose;
    fogPowerUniform.first = 0;
    fogMultUniform.first = 0;
    fogOffsetUniform.first = 0;
    fogColorUniform.first = aquarium->fogUniforms.fogColor;
}

void OutsideModelGL::init()
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
    lightColorUniform.second =
        contextGL->getUniformLocation(programGL->getProgramId(), "lightColor");
    specularUniform.second  = contextGL->getUniformLocation(programGL->getProgramId(), "specular");
    ambientUniform.second   = contextGL->getUniformLocation(programGL->getProgramId(), "ambient");
    shininessUniform.second = contextGL->getUniformLocation(programGL->getProgramId(), "shininess");
    specularFactorUniform.second =
        contextGL->getUniformLocation(programGL->getProgramId(), "specularFactor");

    fogPowerUniform.second  = contextGL->getUniformLocation(programGL->getProgramId(), "fogPower");
    fogMultUniform.second   = contextGL->getUniformLocation(programGL->getProgramId(), "fogMult");
    fogOffsetUniform.second = contextGL->getUniformLocation(programGL->getProgramId(), "fogOffset");
    fogColorUniform.second  = contextGL->getUniformLocation(programGL->getProgramId(), "fogColor");

    diffuseTexture.first    = static_cast<TextureGL *>(textureMap["diffuse"]);
    diffuseTexture.second   = contextGL->getUniformLocation(programGL->getProgramId(), "diffuse");

    positionBuffer.first  = static_cast<BufferGL *>(bufferMap["position"]);
    positionBuffer.second = contextGL->getAttribLocation(programGL->getProgramId(), "position");
    normalBuffer.first    = static_cast<BufferGL *>(bufferMap["normal"]);
    normalBuffer.second   = contextGL->getAttribLocation(programGL->getProgramId(), "normal");
    texCoordBuffer.first  = static_cast<BufferGL *>(bufferMap["texCoord"]);
    texCoordBuffer.second = contextGL->getAttribLocation(programGL->getProgramId(), "texCoord");

    indicesBuffer = static_cast<BufferGL *>(bufferMap["indices"]);
}

void OutsideModelGL::draw()
{
    contextGL->drawElements(indicesBuffer);
}

void OutsideModelGL::preDraw() const
{
    mProgram->setProgram();
    contextGL->enableBlend(mBlend);

    ProgramGL *programGL = static_cast<ProgramGL *>(mProgram);
    contextGL->bindVAO(programGL->getVAOId());

    contextGL->setAttribs(positionBuffer.first, positionBuffer.second);
    contextGL->setAttribs(normalBuffer.first, normalBuffer.second);
    contextGL->setAttribs(texCoordBuffer.first, texCoordBuffer.second);

    contextGL->setIndices(indicesBuffer);

    contextGL->setUniform(viewInverseUniform.second, viewInverseUniform.first, GL_FLOAT_MAT4);
    contextGL->setUniform(lightWorldPosUniform.second, lightWorldPosUniform.first, GL_FLOAT_VEC3);
    contextGL->setUniform(lightColorUniform.second, lightColorUniform.first, GL_FLOAT_VEC4);
    contextGL->setUniform(specularUniform.second, specularUniform.first, GL_FLOAT_VEC4);
    contextGL->setUniform(shininessUniform.second, &shininessUniform.first, GL_FLOAT);
    contextGL->setUniform(specularFactorUniform.second, &specularFactorUniform.first, GL_FLOAT);
    contextGL->setUniform(ambientUniform.second, ambientUniform.first, GL_FLOAT_VEC4);
    contextGL->setUniform(fogPowerUniform.second, &fogPowerUniform.first, GL_FLOAT);
    contextGL->setUniform(fogMultUniform.second, &fogMultUniform.first, GL_FLOAT);
    contextGL->setUniform(fogOffsetUniform.second, &fogOffsetUniform.first, GL_FLOAT);
    contextGL->setUniform(fogColorUniform.second, fogColorUniform.first, GL_FLOAT_VEC4);

    contextGL->setTexture(diffuseTexture.first, diffuseTexture.second, 0);
}

void OutsideModelGL::updatePerInstanceUniforms(ViewUniforms *viewUniforms)
{
    contextGL->setUniform(worldUniform.second, worldUniform.first, GL_FLOAT_MAT4);
    contextGL->setUniform(worldViewProjectionUniform.second, worldViewProjectionUniform.first,
                          GL_FLOAT_MAT4);
    contextGL->setUniform(worldInverseTransposeUniform.second, worldInverseTransposeUniform.first,
                          GL_FLOAT_MAT4);
}
