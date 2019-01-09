#
# Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
cmake_minimum_required(VERSION 3.0)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Release)

OPTION(_NDEBUG "skip ASSERT of the project" OFF)
if(_NDEBUG)
  ADD_DEFINITIONS(-DNDEBUG)
endif(_NDEBUG)

OPTION(angle "create angle context" OFF)
if(angle)
  ADD_DEFINITIONS(-DGL_GLEXT_PROTOTYPES)
  ADD_DEFINITIONS(-DEGL_EGL_PROTOTYPES)
endif(angle)

OPTION(dawn "create dawn context" OFF)
if(dawn)
  ADD_DEFINITIONS(-D_ITERATOR_DEBUG_LEVEL=0)
  ADD_DEFINITIONS(-DDAWN_ENABLE_BACKEND_METAL)
endif(dawn)

if (UNIX AND NOT APPLE)
	set (LINUX TRUE)
endif()

project( aquarium )

include_directories(${CMAKE_SOURCE_DIR}/../thirdparty/stb)
include_directories(${CMAKE_SOURCE_DIR}/../thirdparty/glfw/include)
include_directories(${CMAKE_SOURCE_DIR}/../thirdparty/rapidjson/include)
  
if(angle)
    include_directories(${CMAKE_SOURCE_DIR}/../thirdparty/angle/include)
	include_directories(${CMAKE_SOURCE_DIR}/../thirdparty/angle/src)
	include_directories(${CMAKE_SOURCE_DIR}/../thirdparty/angle/util)
else()
include_directories(${CMAKE_SOURCE_DIR}/../thirdparty/glad/include)
# Glad
add_library(glad STATIC
  ${CMAKE_SOURCE_DIR}/../thirdparty/glad/src/glad.c
  ${CMAKE_SOURCE_DIR}/../thirdparty/glad/include/glad/glad.h
  ${CMAKE_SOURCE_DIR}/../thirdparty/glad/include/KHR/khrplatform.h
)
endif()

if(dawn)
    include_directories(${CMAKE_SOURCE_DIR}/../thirdparty/dawn/third_party/shaderc/libshaderc/include)
    include_directories(${CMAKE_SOURCE_DIR}/../thirdparty/dawn/src)
	include_directories(${CMAKE_SOURCE_DIR}/../thirdparty/dawn/src/include)
	include_directories(${CMAKE_SOURCE_DIR}/../thirdparty/dawn/thirdparty)
	include_directories(${CMAKE_SOURCE_DIR}/../thirdparty/dawn/examples)
	include_directories(${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/gen)
	include_directories(${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/gen)
endif()

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/../thirdparty/glfw ${CMAKE_BINARY_DIR}/glfw)

include_directories(${CMAKE_SOURCE_DIR}/../thirdparty/rapidjson)

set(SOURCE_FILES
src/Aquarium.h
src/ASSERT.h
src/Buffer.h
src/Context.h
src/Context.cpp
src/ContextFactory.h
src/ContextFactory.cpp
src/FishModel.h
src/FPSTimer.cpp
src/FPSTimer.h
src/GenericModel.h
src/InnerModel.h
src/Matrix.h
src/Model.h
src/Model.cpp
src/OutsideModel.h
src/Program.h
src/Program.cpp
src/SeaweedModel.h
src/Texture.h
src/Texture.cpp
src/opengl/BufferGL.h
src/opengl/BufferGL.cpp
src/opengl/ContextGL.h
src/opengl/ContextGL.cpp
src/opengl/FishModelGL.h
src/opengl/FishModelGL.cpp
src/opengl/GenericModelGL.h
src/opengl/GenericModelGL.cpp
src/opengl/InnerModelGL.h
src/opengl/InnerModelGL.cpp
src/opengl/OutsideModelGL.h
src/opengl/OutsideModelGL.cpp
src/opengl/ProgramGL.h
src/opengl/ProgramGL.cpp
src/opengl/SeaweedModelGL.h
src/opengl/SeaweedModelGL.cpp
src/opengl/TextureGL.h
src/opengl/TextureGL.cpp
src/Aquarium.cpp
src/Main.cpp
)

set(DAWN_FILES
src/dawn/BufferDawn.h
src/dawn/BufferDawn.cpp
src/dawn/ContextDawn.h
src/dawn/ContextDawn.cpp
src/dawn/FishModelDawn.h
src/dawn/FishModelDawn.cpp
src/dawn/InnerModelDawn.h
src/dawn/InnerModelDawn.cpp
src/dawn/GenericModelDawn.h
src/dawn/GenericModelDawn.cpp
src/dawn/OutsideModelDawn.h
src/dawn/OutsideModelDawn.cpp
src/dawn/SeaweedModelDawn.h
src/dawn/SeaweedModelDawn.cpp
src/dawn/TextureDawn.h
src/dawn/TextureDawn.cpp
src/dawn/ProgramDawn.h
src/dawn/ProgramDawn.cpp
)

if(dawn)
add_executable(aquarium
${SOURCE_FILES} ${DAWN_FILES}
)
else()
add_executable( aquarium
${SOURCE_FILES} )
endif()

if(angle AND WIN32)
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/angle/out/Debug/libEGL.dll.lib optimized ${CMAKE_SOURCE_DIR}/../thirdparty/angle/out/Release/libEGL.dll.lib)
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/angle/out/Debug/libGLESv2.dll.lib optimized ${CMAKE_SOURCE_DIR}/../thirdparty/angle/out/Release/libGLESv2.dll.lib)
	target_link_libraries(aquarium glfw)
elseif(angle AND LINUX)
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/angle/out/Debug/libEGL.so optimized ${CMAKE_SOURCE_DIR}/../thirdparty/angle/out/Release/libEGL.so)
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/angle/out/Debug/libGLESv2.so optimized ${CMAKE_SOURCE_DIR}/../thirdparty/angle/out/Release/libGLESv2.so)
	target_link_libraries(aquarium glfw)
elseif(angle AND APPLE)
    target_link_libraries(aquarium  "-framework Metal" "-framework QuartzCore")
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/angle/out/Debug/libEGL.dll.dylib optimized ${CMAKE_SOURCE_DIR}/../thirdparty/angle/out/Release/libEGL.dylib)
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/angle/out/Debug/libGLESv2.dll.dylib optimized ${CMAKE_SOURCE_DIR}/../thirdparty/angle/out/Release/libGLESv2.dylib)
	target_link_libraries(aquarium glfw)
elseif(dawn AND WIN32)
    target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/libdawn.dll.lib optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/libdawn.dll.lib)
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/libdawn_native.dll.lib optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/libdawn_native.dll.lib)
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/libshaderc.dll.lib optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/libshaderc.dll.lib)
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/libdawn_wire.dll.lib optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/libdawn_wire.dll.lib)
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/obj/dawn_utils.lib optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/obj/dawn_utils.lib)
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/obj/dawn_sample_utils.lib optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/obj/dawn_sample_utils.lib)
    target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/obj/dawn_common.lib optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/obj/dawn_common.lib)
	target_link_libraries(aquarium glad glfw)
elseif(dawn AND LINUX)
    target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/libdawn.so optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/libdawn.so)
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/libdawn_native.so optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/libdawn_native.so)
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/libshaderc.so optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/libshaderc.so)
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/libdawn_wire.so optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/libdawn_wire.so)
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/obj/libdawn_utils.a optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/obj/libdawn_utils.a)
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/obj/libdawn_sample_utils.a optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/obj/libdawn_sample_utils.a)
    target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/obj/libdawn_common.a optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/obj/libdawn_common.a)
	target_link_libraries(aquarium glad glfw)
elseif(dawn AND APPLE)
    target_link_libraries(aquarium  "-framework Metal" "-framework QuartzCore")
    target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/libdawn.dylib optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/libdawn.dylib)
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/libdawn_native.dylib optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/libdawn_native.dylib)
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/libshaderc.dylib optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/libshaderc.dylib)
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/libdawn_wire.dylib optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/libdawn_wire.dylib)
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/obj/libdawn_utils.a optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/obj/libdawn_utils.a)
	target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/obj/libdawn_sample_utils.a optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/obj/libdawn_sample_utils.a)
    target_link_libraries(aquarium debug ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Debug/obj/libdawn_common.a optimized ${CMAKE_SOURCE_DIR}/../thirdparty/dawn/out/Release/obj/libdawn_common.a)
	target_link_libraries(aquarium glad glfw)
	else ()
	target_link_libraries(aquarium glad glfw)
endif()
