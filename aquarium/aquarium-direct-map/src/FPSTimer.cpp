//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FPSTimer.cpp: Implement fps timer. Update fps of each frame.

#include "FPSTimer.h"

#include <cmath>

FPSTimer::FPSTimer()
    : totalTime_(static_cast<float>(NUM_FRAMES_TO_AVERAGE)),
    timeTable_(NULL),
    timeTableCursor_(0),
    instantaneousFPS(0.0f),
    averageFPS(0.0f)
{
    for (int i = 0; i < NUM_FRAMES_TO_AVERAGE; ++i)
    {
        timeTable_.push_back(1.0f);
    }
}

void FPSTimer::update(float elapsedTime)
{
    totalTime_ += elapsedTime - timeTable_[timeTableCursor_];
    timeTable_[timeTableCursor_] = elapsedTime;

    ++timeTableCursor_;
    if (timeTableCursor_ == NUM_FRAMES_TO_AVERAGE)
    {
        timeTableCursor_ = 0;
    }

    instantaneousFPS = floor(1.0f / elapsedTime + 0.5f);
    averageFPS = floor((1.0f / (totalTime_ / static_cast<float>(NUM_FRAMES_TO_AVERAGE))) + 0.5f);
}
