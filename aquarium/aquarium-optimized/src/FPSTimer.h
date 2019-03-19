//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FPSTimer.h: Define fps timer.

#pragma once
#ifndef FPS_TIMER
#define FPS_TIMER 1

#include <vector>

constexpr int NUM_FRAMES_TO_AVERAGE = 16;

class FPSTimer
{
public:
  FPSTimer();

  void update(float elapsedTime);
  float getAverageFPS() const { return mAverageFPS; }
  float getInstantaneousFPS() const { return mInstantaneousFPS; }

private:
  float mTotalTime;
  std::vector<float> mTimeTable;
  int mTimeTableCursor;
  float mInstantaneousFPS;
  float mAverageFPS;
};

#endif
