//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Program.h: Define base class for Programs of specific backends.

#pragma once
#ifndef PROGRAM_H
#define PROGRAM_H 1

#include <string>
#include <vector>
#include "Aquarium.h"
#include "Buffer.h"
#include "Texture.h"

enum UNIFORMNAME : short;

class Program
{
  public:
    Program(){};
    Program(std::string vertexShader, std::string fragmentShader)
        : vId(vertexShader), fId(fragmentShader)
    {
    }
    virtual ~Program(){};
    virtual void setProgram();

  protected:
    std::string vId;
    std::string fId;
};

#endif // !PROGRAM_H

