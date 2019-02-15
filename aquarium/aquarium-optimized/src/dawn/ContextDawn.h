//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// ContextDawn.h : Defines the accessing to graphics API of Dawn.

#pragma once
#ifndef CONTEXTDAWN_H
#define CONTEXTDAWN_H

#include "../Context.h"

#include "dawn/dawncpp.h"
#include "utils/DawnHelpers.h"

#include "GLFW/glfw3.h"

class TextureDawn;
class BufferDawn;
class ProgramDawn;

class ContextDawn : public Context
{
  public:
      struct Attribute
      {
          int shaderLocation;
          int bindingSlot;
          dawn::VertexFormat format;
          int offset;
      };

      struct Input
      {
          int bindingSlot;
          int stride;
          dawn::InputStepMode stepMode;
      };

    ContextDawn();
    ~ContextDawn();
    bool createContext(std::string backend, bool enableMSAA) override;
    void setWindowTitle(const std::string &text) override;
    bool ShouldQuit() override;
    void KeyBoardQuit() override;
    void DoFlush() override;
    void Terminate() override;

    void preFrame() override;

    Model *createModel(Aquarium* aquarium, MODELGROUP type, MODELNAME name, bool blend) override;
    Buffer *createBuffer(int numComponents,
        const std::vector<float> &buffer,
        bool isIndex) override;
    Buffer *createBuffer(int numComponents,
        const std::vector<unsigned short> &buffer,
        bool isIndex) override;

    Program *createProgram(std::string vId, std::string fId) override;

    Texture *createTexture(std::string name, std::string url) override;
    Texture *createTexture(std::string name, const std::vector<std::string> &urls) override;
    dawn::Texture createTexture(const dawn::TextureDescriptor &descriptor) const;
    dawn::Sampler createSampler(const dawn::SamplerDescriptor &descriptor) const;
    dawn::Buffer createBufferFromData(const void* pixels, int size, dawn::BufferUsageBit usage) const;
    dawn::BufferCopyView createBufferCopyView(const dawn::Buffer &buffer,
        uint32_t offset,
        uint32_t rowPitch,
        uint32_t imageHeight) const;
    dawn::CommandBuffer copyBufferToTexture(const dawn::BufferCopyView &bufferCopyView,
                                            const dawn::TextureCopyView &textureCopyView,
                                            const dawn::Extent3D &ext3D) const;
    void submit(int numCommands, const dawn::CommandBuffer& commands) const;

    dawn::TextureCopyView createTextureCopyView(dawn::Texture texture,
                                                uint32_t level,
                                                uint32_t slice,
                                                dawn::Origin3D origin);
    dawn::ShaderModule createShaderModule(dawn::ShaderStage stage, const std::string &str) const;
    dawn::BindGroupLayout  MakeBindGroupLayout(
        std::initializer_list<dawn::BindGroupLayoutBinding> bindingsInitializer) const;
    dawn::PipelineLayout MakeBasicPipelineLayout(
        std::vector<dawn::BindGroupLayout> bindingsInitializer) const;
    dawn::InputState createInputState(std::initializer_list<Attribute> attributeInitilizer,
        std::initializer_list<Input> inputInitilizer) const;
    dawn::RenderPipeline createRenderPipeline(dawn::PipelineLayout pipelineLayout, ProgramDawn* programDawn, dawn::InputState inputState, bool enableBlend) const;
    dawn::TextureView createDepthStencilView() const;
    dawn::Buffer createBuffer(uint32_t size, dawn::BufferUsageBit bit) const;
    void setBufferData(const dawn::Buffer& buffer, uint32_t start, uint32_t size, const void* pixels) const;
    dawn::BindGroup makeBindGroup(
        const dawn::BindGroupLayout &layout,
        std::initializer_list<utils::BindingInitializationHelper> bindingsInitializer) const;
    void GetNextRenderPassDescriptor(dawn::Texture *backbuffer,
                                     dawn::RenderPassDescriptor *info) const;

    void initGeneralResources(Aquarium* aquarium) override;
    void updateWorldlUniforms(Aquarium* aquarium) override;
    dawn::Device getDevice() const { return device; }

    dawn::RenderPassEncoder pass;
    dawn::BindGroupLayout groupLayoutGeneral;
    dawn::BindGroup bindGroupGeneral;
    dawn::BindGroupLayout groupLayoutWorld;
    dawn::BindGroup bindGroupWorld;

  private:
    GLFWwindow *mWindow;

    dawn::Device device;
    dawn::Queue queue;
    dawn::SwapChain swapchain;
    dawn::CommandBufferBuilder commandBufferBuilder;
    dawn::RenderPassDescriptor renderPassDescriptor;

    dawn::Texture mBackbuffer;
    dawn::TextureView mDepthStencilView;
    dawn::RenderPipeline mPipeline;
    dawn::BindGroup mBindGroup;
    dawn::TextureFormat mPreferredSwapChainFormat;

    dawn::Buffer lightWorldPositionBuffer;
    dawn::Buffer lightBuffer;
    dawn::Buffer fogBuffer;
};

#endif
