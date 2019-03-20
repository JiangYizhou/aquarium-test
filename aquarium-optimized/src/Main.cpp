//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Main.cpp: Entry class of Aquarium.

#include "Aquarium.h"

int main(int argc, char **argv) {
    Aquarium aquarium;
    aquarium.init(argc, argv);
    aquarium.display();

    return 0;
}
