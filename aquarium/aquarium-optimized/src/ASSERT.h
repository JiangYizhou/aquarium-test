//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#pragma once
#ifndef ASSERT_H
#define ASSERT_H 1

//TODO(yizhou) : replace this ASSERT by the code template of ANGLE or Chromium
#ifndef _NDEBUG
#define	ASSERT(expression)				{	\
		if (!(expression))				{	\
			printf("Assertion(%s) failed: file \"%s\", line %d\n", \
				#expression, __FILE__, __LINE__); \
            abort();				        \
		}									\
	}
#else
#define ASSERT(expression) expression
#endif

#ifndef _NDEBUG
#define	SWALLOW_ERROR(expression)				{	\
		if (!(expression))				{	\
			printf("Assertion(%s) failed: file \"%s\", line %d\n", \
				#expression, __FILE__, __LINE__); \
		}									      \
	}
#else
#define SWALLOW_ERROR(expression) expression
#endif

#endif // !ASSERT_H
