//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Model.h: Define base Class Model for all of the models.
// Contains programs, textures and buffers info of models.
// Apply program for its model. Update uniforms, textures
// and buffers for each frame.


#ifndef MODEL_H
#define MODEL_H 1

#include <cstdio>
#include <cstring>
#include <vector>

#include "Aquarium.h"

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
        : mProgram(nullptr), mBlend(blend), mName(name) {};
    virtual ~Model();
    virtual void preDraw() const     = 0;
    virtual void updatePerInstanceUniforms(ViewUniforms* viewUniforms) = 0;
    virtual void draw() = 0;

    void setProgram(Program *program);
    virtual void init() = 0;

    std::vector<std::vector<float>> worldmatrices;
    std::unordered_map<std::string, Texture *> textureMap;
    std::unordered_map<std::string, Buffer *> bufferMap;

  protected:
    Program *mProgram;
    bool mBlend;
    MODELNAME mName;

  private:
    //MODELGROUP mType;
};

#endif
