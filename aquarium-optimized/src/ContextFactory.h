//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#pragma once

#ifndef CONTEXTFACTORY
#define CONTEXTFACTORY 1
#include <string>
#include "Context.h"

class Context;
enum BACKENDTYPE : short;

class ContextFactory
{
  public:
    ContextFactory();
    ~ContextFactory();
    Context *createContext(BACKENDTYPE backendType);

  private:
    Context *context;
};

#endif
