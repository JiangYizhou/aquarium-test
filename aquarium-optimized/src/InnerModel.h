//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// InnerModel.h: Define inner model.

#pragma once
#ifndef INNERMODEL_H
#define INNERMODEL_H 1

#include "Model.h"

class InnerModel : public Model
{
  public:
    InnerModel(MODELGROUP type, MODELNAME name, bool blend) : Model(type, name, blend){};
};

#endif // !INNERMODEL_H
