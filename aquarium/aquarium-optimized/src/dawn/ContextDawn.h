// ContextDawn.h : Defines the accessing to graphics API of Dawn.

#pragma once
#ifndef CONTEXTDAWN_H
#define CONTEXTDAWN_H

#include "../Context.h"

#include "dawn/dawncpp.h"
#include "shaderc/shaderc.hpp"

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

      struct BindingInitializationHelper {
          BindingInitializationHelper(uint32_t binding, const dawn::Sampler& sampler);
          BindingInitializationHelper(uint32_t binding, const dawn::TextureView& textureView);
          BindingInitializationHelper(uint32_t binding,
              const dawn::Buffer& buffer,
              uint32_t offset,
              uint32_t size);

          dawn::BindGroupBinding GetAsBinding() const;

          uint32_t binding;
          dawn::Sampler sampler;
          dawn::TextureView textureView;
          dawn::Buffer buffer;
          uint32_t offset = 0;
          uint32_t size = 0;
      };

    ContextDawn();
    ~ContextDawn();
    bool createContext() override;
    void setWindowTitle(const std::string &text) override;
    bool ShouldQuit() override;
    void KeyBoardQuit() override;
    void DoFlush() override;
    void Terminate() override;

    void resetState() override;
    void enableBlend(bool flag) const override;

    Model *createModel(Aquarium* aquarium, MODELGROUP type, MODELNAME name, bool blend) override;
    void setTexture(const TextureDawn *texture, int index, int unit) const;
    void setAttribs(BufferDawn *bufferGL, int index) const;
    void setIndices(BufferDawn *bufferGL) const;
    void drawElements(BufferDawn *buffer) const;

    Buffer *createBuffer(int numComponents,
        const std::vector<float> &buffer,
        bool isIndex) override;
    Buffer *createBuffer(int numComponents,
        const std::vector<unsigned short> &buffer,
        bool isIndex) override;
    void generateBuffer(unsigned int *buf);
    void deleteBuffer(unsigned int *buf);
    void bindBuffer(unsigned int target, unsigned int buf);
    void uploadBuffer(unsigned int target, const std::vector<float> &buf);
    void uploadBuffer(unsigned int target, const std::vector<unsigned short> &buf);

    Program *createProgram(std::string vId, std::string fId) override;
    void generateProgram(unsigned int *program);
    void setProgram(unsigned int program);
    void deleteProgram(unsigned int *program);
    bool compileProgram(unsigned int programId,
        const string &VertexShaderCode,
        const string &FragmentShaderCode);

    Texture *createTexture(std::string name, std::string url) override;
    Texture *createTexture(std::string name, const std::vector<std::string> &urls) override;
    void generateTexture(unsigned int *texture);
    void bindTexture(unsigned int target, unsigned int texture);
    void deleteTexture(unsigned int *texture);
    
    dawn::Texture createTexture(const dawn::TextureDescriptor &descriptor) const;
    dawn::Sampler createSampler(const dawn::SamplerDescriptor &descriptor) const;
    dawn::Buffer createBufferFromData(const void* pixels, int size, dawn::BufferUsageBit usage) const;
    dawn::BufferCopyView createBufferCopyView(const dawn::Buffer &buffer,
        uint32_t offset,
        uint32_t rowPitch,
        uint32_t imageHeight) const;
    dawn::TextureCopyView CreateTextureCopyView(const dawn::Texture &texture,
        uint32_t level,
        uint32_t slice,
        dawn::Origin3D origin,
        dawn::TextureAspect aspect) const;
    dawn::CommandBuffer copyBufferToTexture(const dawn::BufferCopyView &bufferCopyView, const dawn::TextureCopyView &textureCopyView, const dawn::Extent3D& ext3D) const;
    void submit(int numCommands, const dawn::CommandBuffer& commands) const;

    dawn::TextureCopyView createTextureCopyView(dawn::Texture texture,
        uint32_t level,
        uint32_t slice,
        dawn::Origin3D origin) const;
    dawn::ShaderModule createShaderModule(dawn::ShaderStage stage, const std::string & str, const std::string & shaderId) const;
    dawn::ShaderModule CreateShaderModuleFromResult(
        const dawn::Device& device,
        const shaderc::SpvCompilationResult& result) const;
    shaderc_shader_kind ShadercShaderKind(dawn::ShaderStage stage) const;
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
    dawn::BindGroup makeBindGroup(const dawn::BindGroupLayout& layout,
        std::initializer_list<BindingInitializationHelper> bindingsInitializer) const;

    void initGeneralResources(Aquarium* aquarium) override;
    void updateWorldlUniforms(Aquarium* aquarium) override;

    dawn::Device getDevice() const { return device; }

    dawn::Buffer lightWorldPositionBuffer;
    dawn::Buffer lightBuffer;
    dawn::Buffer fogBuffer;

    dawn::BindGroupLayout groupLayoutGeneral;
    dawn::BindGroup bindGroupGeneral;

    dawn::BindGroupLayout groupLayoutWorld;
    dawn::BindGroup bindGroupWorld;
    
    void GetNextRenderPassDescriptor(
        dawn::Texture* backbuffer,
        dawn::RenderPassDescriptor* info) const;
    dawn::SwapChain swapchain;

    dawn::RenderPassDescriptor renderPassDescriptor;

  private:
    GLFWwindow *mWindow;
    dawn::Device device;

    dawn::TextureView depthStencilView;
    dawn::RenderPipeline pipeline;
    dawn::BindGroup bindGroup;
    dawn::Queue queue;
    dawn::TextureFormat preferredSwapChainFormat;

    dawn::Buffer indexBuffer;
    dawn::Buffer vertexBuffer;
    dawn::Texture texture;
    dawn::Sampler sampler;

    dawn::Texture backbuffer;
};

#endif
