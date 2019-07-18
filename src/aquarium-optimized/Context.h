//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Context.h: Defines the accessing to graphics API of a graphics backend.

#pragma once
#ifndef Context_H
#define Context_H 1

#include <bitset>
#include <string>
#include <vector>

#include "Aquarium.h"
#include "ResourceHelper.h"

#include "common/FPSTimer.h"

class Aquarium;
class Program;
class Buffer;
class Texture;
class Model;

enum BACKENDTYPE : short;
enum MODELGROUP : short;
enum MODELNAME : short;
enum TOGGLE : short;

struct Global;

class Context
{
  public:
    Context() {}
    virtual ~Context() {}
    virtual bool initialize(
        BACKENDTYPE backend,
        const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset)              = 0;
    virtual Texture *createTexture(std::string name, std::string url)                         = 0;
    virtual Texture *createTexture(std::string name, const std::vector<std::string> &urls)    = 0;
    virtual Buffer *createBuffer(int numComponents, std::vector<float> &buffer, bool isIndex) = 0;
    virtual Buffer *createBuffer(int numComponents,
                                 std::vector<unsigned short> &buffer,
                                 bool isIndex)                                                = 0;
    virtual Program *createProgram(std::string vId, std::string fId)                          = 0;
    virtual void setWindowTitle(const std::string &text)                                      = 0;
    virtual bool ShouldQuit()                                                                 = 0;
    virtual void KeyBoardQuit()                                                               = 0;
    virtual void DoFlush()                                                                    = 0;
    virtual void Terminate()                                                                  = 0;
    virtual void FlushInit() {}
    virtual void preFrame()   = 0;
    virtual void showWindow() = 0;
    virtual void showFPS(const FPSTimer& fpsTimer)    = 0;
    virtual void destoryImgUI() = 0;

    int getClientWidth() const { return mClientWidth; }
    int getclientHeight() const { return mClientHeight; }
    std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> getAvailableToggleBitset()
    {
        return mAvailableToggleBitset;
    }

    virtual Model *createModel(Aquarium *aquarium, MODELGROUP type, MODELNAME name, bool blend) = 0;

    virtual void initGeneralResources(Aquarium *aquarium) {}
    virtual void updateWorldlUniforms(Aquarium *aquarium) {}

    const ResourceHelper *getResourceHelper() { return mResourceHelper; }

  protected:
    int mClientWidth;
    int mClientHeight;

    ResourceHelper *mResourceHelper;

    std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> mAvailableToggleBitset;
    virtual void initAvailableToggleBitset(BACKENDTYPE backendType) = 0;
};

#endif
