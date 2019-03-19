//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FishModelGL.h: Defines fish model of OpenGL.

#ifndef FishModelGL_H
#define FishModelGL_H 1

#include <string>

#include "../FishModel.h"

class TextureGL;
class BufferGL;

class FishModelGL : public FishModel
{
  public:
    FishModelGL(const ContextGL *context, Aquarium *aquarium, MODELGROUP type, MODELNAME name, bool blend);
    void preDraw() const override;
    void updatePerInstanceUniforms(ViewUniforms *viewUniforms) override;
    void updateFishCommonUniforms(float fishLength,
                                  float fishBendAmount,
                                  float fishWaveLength) override;
    void init() override;
    void draw() override;

    void updateFishPerUniforms(float x,
                               float y,
                               float z,
                               float nextX,
                               float nextY,
                               float nextZ,
                               float scale,
                               float time) override;

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
    std::pair<float, int> fishLengthUniform;
    std::pair<float, int> fishWaveLengthUniform;
    std::pair<float, int> fishBendAmountUniform;

    std::pair<float[3], int> worldPositionUniform;
    std::pair<float[3], int> nextPositionUniform;
    std::pair<float, int> scaleUniform;
    std::pair<float, int> timeUniform;

    std::pair<TextureGL *, int> diffuseTexture;
    std::pair<TextureGL *, int> normalTexture;
    std::pair<TextureGL *, int> reflectionTexture;
    std::pair<TextureGL *, int> skyboxTexture;

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
