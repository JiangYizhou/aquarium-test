//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Model.h: Define Class Model for all of the models.
// Contains programs, textures and buffers info of models.

#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <unordered_map>

#include "Buffer.h"
#include "Program.h"
#include "Texture.h"

struct FishConst;
struct FishPer;
struct GenericConst;
struct GenericPer;

class Model
{
public:
  Model() {}
  ~Model();
  Model(Program *program,
        std::unordered_map<std::string, AttribBuffer *> *arrays,
        std::unordered_map<std::string, Texture *> *textures);

  void drawPrep(const GenericConst &constUniforms);
  void drawPrep(const FishConst &fishConst);
  void draw(const GenericPer &perUniforms);
  void draw(const FishPer &fishPer);

private:
  void setBuffers(std::unordered_map<std::string, AttribBuffer *> *arrays);
  void setBuffer(const std::string &name, AttribBuffer *array);
  void applyBuffers() const;
  void applyTextures() const;
  void drawFunc();

  std::unordered_map<std::string, Buffer *> buffers;
  std::unordered_map<std::string, Texture *> *textures;
  Program *program;

  GLenum mode;

};

#endif
