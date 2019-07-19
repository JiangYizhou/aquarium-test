//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// OutsideModelGL.h: Define the outside model of OpenGL.

#pragma once
#ifndef OutsideModelGL_H
#define OutsideModelGL_H 1

#include "ContextGL.h"
#include "ProgramGL.h"

#include "../Model.h"

class OutsideModelGL : public Model
{
  public:
    OutsideModelGL(const ContextGL *context,
                   Aquarium *aquarium,
                   MODELGROUP type,
                   MODELNAME name,
                   bool blend);
    void prepareForDraw() const override;
    void updatePerInstanceUniforms(const WorldUniforms &worldUniforms) override;
    void init() override;
    void draw() override;

    std::pair<float *, int> worldViewProjectionUniform;
    std::pair<float *, int> worldUniform;
    std::pair<float *, int> worldInverseTransposeUniform;

    std::pair<float *, int> viewInverseUniform;
    std::pair<float *, int> lightWorldPosUniform;
    std::pair<float *, int> lightColorUniform;
    std::pair<float *, int> specularUniform;
    std::pair<float, int> shininessUniform;
    std::pair<float, int> specularFactorUniform;

    std::pair<float *, int> ambientUniform;

    std::pair<float, int> fogPowerUniform;
    std::pair<float, int> fogMultUniform;
    std::pair<float, int> fogOffsetUniform;
    std::pair<float *, int> fogColorUniform;

    std::pair<TextureGL *, int> diffuseTexture;

    std::pair<BufferGL *, int> positionBuffer;
    std::pair<BufferGL *, int> normalBuffer;
    std::pair<BufferGL *, int> texCoordBuffer;

    BufferGL *indicesBuffer;

  private:
    const ContextGL *contextGL;
};

#endif
