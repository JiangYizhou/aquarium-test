//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// OutsideModel.h: Define outside model.

#pragma once
#ifndef OUTSIDEMODEL_H
#define OUTSIDEMODEL_H 1

#include "Model.h"

class OutsideModel : public Model
{
  public:
    OutsideModel(MODELGROUP type, MODELNAME name, bool blend) : Model(type, name, blend){};
};

#endif
