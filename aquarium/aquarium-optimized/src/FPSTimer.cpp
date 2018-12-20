//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FPSTimer.cpp: Implement fps timer.

#include "FPSTimer.h"
#include <cmath>

FPSTimer::FPSTimer()
    : mTotalTime(static_cast<float>(NUM_FRAMES_TO_AVERAGE)),
    mTimeTable(NULL),
    mTimeTableCursor(0),
    mInstantaneousFPS(0.0f),
    mAverageFPS(0.0f)
{
    for (int i = 0; i < NUM_FRAMES_TO_AVERAGE; ++i)
    {
        mTimeTable.push_back(1.0f);
    }
}

void FPSTimer::update(float elapsedTime)
{
    mTotalTime += elapsedTime - mTimeTable[mTimeTableCursor];
    mTimeTable[mTimeTableCursor] = elapsedTime;

    ++mTimeTableCursor;
    if (mTimeTableCursor == NUM_FRAMES_TO_AVERAGE)
    {
        mTimeTableCursor = 0;
    }

    mInstantaneousFPS = floor(1.0f / elapsedTime + 0.5f);
    mAverageFPS = floor((1.0f / (mTotalTime / static_cast<float>(NUM_FRAMES_TO_AVERAGE))) + 0.5f);
}
