//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// SeaweedModelGL.h: Define seaweed model of OpeGL.

#pragma once
#ifndef SEAWEEDMODELGL_H
#define SEAWEEDMODELGL_H 1

#include "../SeaweedModel.h"
#include "ProgramGL.h"
#include "ContextGL.h"

class SeaweedModelGL : public SeaweedModel
{
  public:
    SeaweedModelGL(const ContextGL *context,
                   Aquarium *aquarium,
                   MODELGROUP type,
                   MODELNAME name,
                   bool blend);
    void preDraw() const override;
    void updatePerInstanceUniforms(ViewUniforms *viewUniforms) override;
    void init() override;
    void draw() override;

    void updateSeaweedModelTime(float time) override;

    std::pair<float *, int> worldUniform;

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

    std::pair<float *, int> viewProjectionUniform;
    std::pair<float, int> timeUniform;

    std::pair<TextureGL *, int> diffuseTexture;

    std::pair<BufferGL *, int> positionBuffer;
    std::pair<BufferGL *, int> normalBuffer;
    std::pair<BufferGL *, int> texCoordBuffer;

    BufferGL * indicesBuffer;

  private:
    const ContextGL *contextGL;
};

#endif // !SEAWEEDMODELGL_H
