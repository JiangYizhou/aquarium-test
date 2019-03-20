//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// GenericModel.h: Defines generic model of OpenGL.

#pragma once
#ifndef GENERICMODELGL_H
#define GENERICMODELGL_H 1

#include "../GenericModel.h"
#include "ContextGL.h"
#include "ProgramGL.h"

class GenericModelGL : public GenericModel
{
  public:
    GenericModelGL(const ContextGL *context,
                   Aquarium* aquarium,
                   MODELGROUP type,
                   MODELNAME name,
                   bool blend);
    void preDraw() const override;
    void updatePerInstanceUniforms(ViewUniforms *viewUniforms) override;
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
    std::pair<TextureGL *, int> normalTexture;

    std::pair<BufferGL *, int> positionBuffer;
    std::pair<BufferGL *, int> normalBuffer;
    std::pair<BufferGL *, int> texCoordBuffer;

    std::pair<BufferGL *, int> tangentBuffer;
    std::pair<BufferGL *, int> binormalBuffer;

    BufferGL * indicesBuffer;

  private:
    const ContextGL *contextGL;
};

#endif
