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

include_directories(${CMAKE_SOURCE_DIR}/third_party/stb)
#include_directories(${CMAKE_SOURCE_DIR}/third_party/glfw/include)
#include_directories(${CMAKE_SOURCE_DIR}/third_party/glad/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/rapidjson/include)
#include_directories(${CMAKE_SOURCE_DIR}/src/common)
#include_directories(${CMAKE_SOURCE_DIR}/src/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Glad
#add_library(glad STATIC
#  ${CMAKE_SOURCE_DIR}/third_party/glad/src/glad.c
#  ${CMAKE_SOURCE_DIR}/third_party/glad/include/glad/glad.h
#  ${CMAKE_SOURCE_DIR}/third_party/glad/include/KHR/khrplatform.h
#)
#set(GLAD_INCLUDE_DIR third_party/glad/include)

# GLFW
#set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
#add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/glfw ${CMAKE_BINARY_DIR}/glfw)

#target_include_directories(glad SYSTEM PUBLIC ${GLAD_INCLUDE_DIR})

#list(APPEND THIRDPARTY_LIBS
#  glad
#)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/third_party/rapidjson)

add_executable( aquarium
src/common/AQUARIUM_ASSERT.h
src/aquarium-optimized/opengl/BufferGL.h
src/aquarium-optimized/opengl/BufferGL.cpp
src/common/FPSTimer.h
src/common/FPSTimer.cpp
src/aquarium-optimized/opengl/ContextGL.h
src/aquarium-optimized/opengl/ContextGL.cpp
src/aquarium-optimized/opengl/FishModelGL.h
src/aquarium-optimized/opengl/FishModelGL.cpp
src/aquarium-optimized/opengl/GenericModelGL.h
src/aquarium-optimized/opengl/GenericModelGL.cpp
src/aquarium-optimized/opengl/InnerModelGL.h
src/aquarium-optimized/opengl/InnerModelGL.cpp
src/aquarium-optimized/opengl/OutsideModelGL.h
src/aquarium-optimized/opengl/OutsideModelGL.cpp
src/aquarium-optimized/opengl/ProgramGL.h
src/aquarium-optimized/opengl/ProgramGL.cpp
src/aquarium-optimized/opengl/SeaweedModelGL.h
src/aquarium-optimized/opengl/SeaweedModelGL.cpp
src/aquarium-optimized/opengl/TextureGL.h
src/aquarium-optimized/opengl/TextureGL.cpp
src/aquarium-optimized/Aquarium.h
src/aquarium-optimized/Aquarium.cpp
src/aquarium-optimized/Buffer.h
src/aquarium-optimized/Context.h
src/aquarium-optimized/ContextFactory.h
src/aquarium-optimized/ContextFactory.cpp
src/aquarium-optimized/FishModel.h
src/aquarium-optimized/Main.cpp
src/aquarium-optimized/Matrix.h
src/aquarium-optimized/Model.h
src/aquarium-optimized/Model.cpp
src/aquarium-optimized/Program.h
src/aquarium-optimized/ResourceHelper.h
src/aquarium-optimized/ResourceHelper.cpp
src/aquarium-optimized/SeaweedModel.h
src/aquarium-optimized/Texture.h
src/aquarium-optimized/Texture.cpp
)

target_link_libraries(aquarium ${THIRDPARTY_LIBS})
SET_PROPERTY(TARGET aquarium APPEND_STRING PROPERTY LINK_FLAGS " -s USE_GLFW=3 -s USE_WEBGL2=1 -s MAIN_MODULE=1 -s FULL_ES3=1 --preload-file ../../assets --preload-file ../../shaders -s TOTAL_MEMORY=256MB -s GL_ASSERTIONS=1 -s GL_DEBUG=1")
#SET_PROPERTY(TARGET aquarium APPEND_STRING PROPERTY LINK_FLAGS " -s USE_GLFW=3 -s USE_WEBGL=1 -s MAIN_MODULE=1 -s FULL_ES2=1")
SET_PROPERTY(TARGET aquarium PROPERTY SUFFIX ".html")
