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
    : Model(type, name, blend), mContextGL(context)
{
    mViewInverseUniform.first   = aquarium->lightWorldPositionUniform.viewInverse;
    mLightWorldPosUniform.first = aquarium->lightWorldPositionUniform.lightWorldPos;

    mWorldUniform.first                 = aquarium->mWorldUniforms.world;
    mWorldViewProjectionUniform.first   = aquarium->mWorldUniforms.worldViewProjection;
    mWorldInverseTransposeUniform.first = aquarium->mWorldUniforms.worldInverseTranspose;

    etaUniform.first = 1.0f;
    tankColorFudgeUniform.first = 0.796f;
    refractionFudgeUniform.first = 3.0f;

    mFogPowerUniform.first  = g_fogPower;
    mFogMultUniform.first   = g_fogMult;
    mFogOffsetUniform.first = g_fogOffset;
    mFogColorUniform.first  = aquarium->fogUniforms.fogColor;
}

void InnerModelGL::init()
{
    ProgramGL *programGL = static_cast<ProgramGL *>(mProgram);
    mWorldViewProjectionUniform.second =
        mContextGL->getUniformLocation(programGL->getProgramId(), "worldViewProjection");
    mWorldUniform.second = mContextGL->getUniformLocation(programGL->getProgramId(), "world");
    mWorldInverseTransposeUniform.second =
        mContextGL->getUniformLocation(programGL->getProgramId(), "worldInverseTranspose");

    mViewInverseUniform.second =
        mContextGL->getUniformLocation(programGL->getProgramId(), "viewInverse");
    mLightWorldPosUniform.second =
        mContextGL->getUniformLocation(programGL->getProgramId(), "lightWorldPos");

    mFogPowerUniform.second = mContextGL->getUniformLocation(programGL->getProgramId(), "fogPower");
    mFogMultUniform.second  = mContextGL->getUniformLocation(programGL->getProgramId(), "fogMult");
    mFogOffsetUniform.second =
        mContextGL->getUniformLocation(programGL->getProgramId(), "fogOffset");
    mFogColorUniform.second = mContextGL->getUniformLocation(programGL->getProgramId(), "fogColor");

    etaUniform.second = mContextGL->getUniformLocation(programGL->getProgramId(), "eta");
    tankColorFudgeUniform.second =
        mContextGL->getUniformLocation(programGL->getProgramId(), "tankColorFudge");
    refractionFudgeUniform.second =
        mContextGL->getUniformLocation(programGL->getProgramId(), "refractionFudge");

    mDiffuseTexture.first  = static_cast<TextureGL *>(mTextureMap["diffuse"]);
    mDiffuseTexture.second = mContextGL->getUniformLocation(programGL->getProgramId(), "diffuse");
    mNormalTexture.first   = static_cast<TextureGL *>(mTextureMap["normalMap"]);
    mNormalTexture.second  = mContextGL->getUniformLocation(programGL->getProgramId(), "normalMap");
    mReflectionTexture.first = static_cast<TextureGL *>(mTextureMap["reflectionMap"]);
    mReflectionTexture.second =
        mContextGL->getUniformLocation(programGL->getProgramId(), "reflectionMap");
    mSkyboxTexture.first  = static_cast<TextureGL *>(mTextureMap["skybox"]);
    mSkyboxTexture.second = mContextGL->getUniformLocation(programGL->getProgramId(), "skybox");

    mPositionBuffer.first  = static_cast<BufferGL *>(mBufferMap["position"]);
    mPositionBuffer.second = mContextGL->getAttribLocation(programGL->getProgramId(), "position");
    mNormalBuffer.first    = static_cast<BufferGL *>(mBufferMap["normal"]);
    mNormalBuffer.second   = mContextGL->getAttribLocation(programGL->getProgramId(), "normal");
    mTexCoordBuffer.first  = static_cast<BufferGL *>(mBufferMap["texCoord"]);
    mTexCoordBuffer.second = mContextGL->getAttribLocation(programGL->getProgramId(), "texCoord");
    mTangentBuffer.first   = static_cast<BufferGL *>(mBufferMap["tangent"]);
    mTangentBuffer.second  = mContextGL->getAttribLocation(programGL->getProgramId(), "tangent");
    mBinormalBuffer.first  = static_cast<BufferGL *>(mBufferMap["binormal"]);
    mBinormalBuffer.second = mContextGL->getAttribLocation(programGL->getProgramId(), "binormal");

    mIndicesBuffer = static_cast<BufferGL *>(mBufferMap["indices"]);
}

void InnerModelGL::draw()
{
    mContextGL->drawElements(mIndicesBuffer);
}

void InnerModelGL::prepareForDraw() const
{
    mProgram->setProgram();
    mContextGL->enableBlend(mBlend);

    ProgramGL *programGL = static_cast<ProgramGL *>(mProgram);
    mContextGL->bindVAO(programGL->getVAOId());

    mContextGL->setAttribs(mPositionBuffer.first, mPositionBuffer.second);
    mContextGL->setAttribs(mNormalBuffer.first, mNormalBuffer.second);
    mContextGL->setAttribs(mTexCoordBuffer.first, mTexCoordBuffer.second);

    mContextGL->setAttribs(mTangentBuffer.first, mTangentBuffer.second);
    mContextGL->setAttribs(mBinormalBuffer.first, mBinormalBuffer.second);

    mContextGL->setIndices(mIndicesBuffer);

    mContextGL->setUniform(mViewInverseUniform.second, mViewInverseUniform.first, GL_FLOAT_MAT4);
    // lightWorldPosition is optimized away on mesa because it's not used by shader
    // mContextGL->setUniform(mLightWorldPosUniform.second, mLightWorldPosUniform.first,
    // GL_FLOAT_VEC3);
    mContextGL->setUniform(mFogPowerUniform.second, &mFogPowerUniform.first, GL_FLOAT);
    mContextGL->setUniform(mFogMultUniform.second, &mFogMultUniform.first, GL_FLOAT);
    mContextGL->setUniform(mFogOffsetUniform.second, &mFogOffsetUniform.first, GL_FLOAT);
    mContextGL->setUniform(mFogColorUniform.second, mFogColorUniform.first, GL_FLOAT_VEC4);
    mContextGL->setUniform(etaUniform.second, &etaUniform.first, GL_FLOAT);
    mContextGL->setUniform(tankColorFudgeUniform.second, &tankColorFudgeUniform.first, GL_FLOAT);
    mContextGL->setUniform(refractionFudgeUniform.second, &refractionFudgeUniform.first, GL_FLOAT);

    mContextGL->setTexture(mDiffuseTexture.first, mDiffuseTexture.second, 0);
    mContextGL->setTexture(mNormalTexture.first, mNormalTexture.second, 1);
    mContextGL->setTexture(mReflectionTexture.first, mReflectionTexture.second, 2);
    mContextGL->setTexture(mSkyboxTexture.first, mSkyboxTexture.second, 3);
}

void InnerModelGL::updatePerInstanceUniforms(WorldUniforms *worldUniforms)
{
    mContextGL->setUniform(mWorldUniform.second, mWorldUniform.first, GL_FLOAT_MAT4);
    mContextGL->setUniform(mWorldViewProjectionUniform.second, mWorldViewProjectionUniform.first,
                           GL_FLOAT_MAT4);
    mContextGL->setUniform(mWorldInverseTransposeUniform.second,
                           mWorldInverseTransposeUniform.first, GL_FLOAT_MAT4);
}
