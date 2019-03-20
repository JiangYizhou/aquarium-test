//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// GenericModel.h: Define generic model.

#pragma once
#ifndef GENERICMODEL_H
#define GENERICMODEL_H 1

#include "Model.h"

class GenericModel : public Model
{
  public:
    GenericModel(MODELGROUP type, MODELNAME name, bool blend) : Model(type, name, blend){};
};

#endif
