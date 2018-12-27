//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Model.h: Define base Class Model for all of the models.
// Contains programs, textures and buffers info of models.
// Apply program for its model. Update uniforms, textures
// and buffers for each frame.

#pragma once
#ifndef MODEL_H
#define MODEL_H 1

#include <vector>

#include "Aquarium.h"
#include "Buffer.h"
#include "Context.h"
#include "Program.h"
#include "Texture.h"

class Aquarium;
class Program;
class Context;
class Texture;
class Buffer;

enum MODELGROUP : short;
enum MODELNAME : short;
struct ViewUniforms;

class Model
{
  public:
    Model();
    Model(MODELGROUP type, MODELNAME name, bool blend)
        : mType(type), mName(name), mBlend(blend), mProgram(nullptr){};
    virtual ~Model();
    virtual void applyUniforms() const     = 0;
    virtual void updatePerInstanceUniforms(ViewUniforms* viewUniforms) = 0;
    virtual void draw() = 0;

    void applyPrograms() const;
    virtual void applyBuffers() const = 0;
    virtual void applyTextures() const = 0;
    void setProgram(Program *program);
    void prepare(const Context *context);
    virtual void init() = 0;

    std::vector<std::vector<float>> worldmatrices;
    std::unordered_map<std::string, Texture *> textureMap;
    std::unordered_map<std::string, Buffer *> bufferMap;

  protected:
    Program *mProgram;
    bool mBlend;
    MODELNAME mName;

  private:
    MODELGROUP mType;
};

#endif
