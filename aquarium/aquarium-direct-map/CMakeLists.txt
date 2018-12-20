#
# Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
cmake_minimum_required(VERSION 3.0)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Release)

OPTION(_NDEBUG "skip ASSERT of the project" ON)
if(_NDEBUG)
  ADD_DEFINITIONS(-D_NDEBUG)
endif(_NDEBUG)

if (UNIX AND NOT APPLE)
	set (LINUX TRUE)
endif()

project( aquarium )

include_directories(${CMAKE_SOURCE_DIR}/../thirdparty/stb)
include_directories(${CMAKE_SOURCE_DIR}/../thirdparty/glfw/include)
include_directories(${CMAKE_SOURCE_DIR}/../thirdparty/glad/include)
include_directories(${CMAKE_SOURCE_DIR}/../thirdparty/rapidjson/include)
  
# Glad
add_library(glad STATIC
  ${CMAKE_SOURCE_DIR}/../thirdparty/glad/src/glad.c
  ${CMAKE_SOURCE_DIR}/../thirdparty/glad/include/glad/glad.h
  ${CMAKE_SOURCE_DIR}/../thirdparty/glad/include/KHR/khrplatform.h
)
set(GLAD_INCLUDE_DIR thirdparty/glad/include)
  
# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/../thirdparty/glfw ${CMAKE_BINARY_DIR}/glfw)

target_include_directories(glad SYSTEM PUBLIC ${GLAD_INCLUDE_DIR})

list(APPEND THIRDPARTY_LIBS
  glad
  glfw
)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/../thirdparty/rapidjson)

add_executable( aquarium 
src/ASSERT.h
src/AttribBuffer.h
src/AttribBuffer.cpp
src/Buffer.h
src/Buffer.cpp
src/FPSTimer.h
src/FPSTimer.cpp
src/Globals.h
src/Main.cpp
src/Matrix.h
src/Model.h
src/Model.cpp
src/Program.h
src/Program.cpp
src/Scene.h
src/Scene.cpp
src/Texture.h
src/Texture.cpp
src/Uniform.h
src/Uniform.cpp
)

target_link_libraries(aquarium ${THIRDPARTY_LIBS})