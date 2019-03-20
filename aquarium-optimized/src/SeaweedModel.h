//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// SeaweedModel.h: Define seaweed model.

#pragma once
#ifndef SEAWEEDMODEL_H
#define SEAWEEDMODEL_H 1

#include "Model.h"

class SeaweedModel : public Model
{
  public:
    SeaweedModel(MODELGROUP type, MODELNAME name, bool blend) : Model(type, name, blend){};

    virtual void updateSeaweedModelTime(float time) = 0;
};

#endif // !SEAWEEDMODEL_H
