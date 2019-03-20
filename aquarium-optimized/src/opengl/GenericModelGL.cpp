//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// GenericModelGL.cpp: Implement generic model of OpenGL.

#include "GenericModelGL.h"

GenericModelGL::GenericModelGL(const ContextGL *context,
                               Aquarium* aquarium,
                               MODELGROUP type,
                               MODELNAME name,
                               bool blend)
    : GenericModel(type, name, blend), contextGL(context)
{
    viewInverseUniform.first = aquarium->viewUniforms.viewInverse;
    lightWorldPosUniform.first = aquarium->lightWorldPositionUniform.lightWorldPos;
    lightColorUniform.first = aquarium->lightUniforms.lightColor;
    specularUniform.first = aquarium->lightUniforms.specular;
    shininessUniform.first = 50.0f;
    specularFactorUniform.first = 1.0f;
    ambientUniform.first = aquarium->lightUniforms.ambient;
    worldUniform.first = aquarium->viewUniforms.world;
    worldViewProjectionUniform.first = aquarium->viewUniforms.worldViewProjection;
    worldInverseTransposeUniform.first = aquarium->viewUniforms.worldInverseTranspose;
    fogPowerUniform.first = g_fogPower;
    fogMultUniform.first = g_fogMult;
    fogOffsetUniform.first = g_fogOffset;
    fogColorUniform.first = aquarium->fogUniforms.fogColor;
}

void GenericModelGL::init()
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

    diffuseTexture.first  = static_cast<TextureGL *>(textureMap["diffuse"]);
    diffuseTexture.second = contextGL->getUniformLocation(programGL->getProgramId(), "diffuse");
    normalTexture.first   = static_cast<TextureGL *>(textureMap["normalMap"]);
    normalTexture.second  = contextGL->getUniformLocation(programGL->getProgramId(), "normalMap");

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

void GenericModelGL::draw()
{
    contextGL->drawElements(indicesBuffer);
}

void GenericModelGL::preDraw() const
{
    mProgram->setProgram();
    contextGL->enableBlend(mBlend);

    ProgramGL *programGL = static_cast<ProgramGL *>(mProgram);
    contextGL->bindVAO(programGL->getVAOId());

    contextGL->setAttribs(positionBuffer.first, positionBuffer.second);
    contextGL->setAttribs(normalBuffer.first, normalBuffer.second);
    contextGL->setAttribs(texCoordBuffer.first, texCoordBuffer.second);

    // diffuseVertexShader doesn't contains tangent and binormal but normalMapVertexShader
    // contains the two buffers.
    if (tangentBuffer.second != -1 && binormalBuffer.second != -1)
    {
        contextGL->setAttribs(tangentBuffer.first, tangentBuffer.second);
        contextGL->setAttribs(binormalBuffer.first, binormalBuffer.second);
    }

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
    // Generic models includes Arch, coral, rock, ship, etc. diffuseFragmentShader doesn't contain
    // normalMap texture but normalMapFragmentShader contains.
    if (normalTexture.second != -1)
    {
        contextGL->setTexture(normalTexture.first, normalTexture.second, 1);
    }
}

void GenericModelGL::updatePerInstanceUniforms(ViewUniforms *viewUniforms)
{
    contextGL->setUniform(worldUniform.second, worldUniform.first, GL_FLOAT_MAT4);
    contextGL->setUniform(worldViewProjectionUniform.second, worldViewProjectionUniform.first,
                          GL_FLOAT_MAT4);
    contextGL->setUniform(worldInverseTransposeUniform.second, worldInverseTransposeUniform.first,
                          GL_FLOAT_MAT4);
}
