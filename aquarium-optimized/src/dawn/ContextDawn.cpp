//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// DeviceDawn.cpp: Implements accessing functions to the graphics API of Dawn.

#include <iostream>
#include <string>
#include <vector>

#include "../Aquarium.h"
#include "BufferDawn.h"
#include "ContextDawn.h"
#include "FishModelDawn.h"
#include "FishModelInstancedDrawDawn.h"
#include "GenericModelDawn.h"
#include "InnerModelDawn.h"
#include "OutsideModelDawn.h"
#include "ProgramDawn.h"
#include "SeaweedModelDawn.h"
#include "TextureDawn.h"

#include <dawn/dawn.h>
#include <dawn/dawn_wsi.h>
#include <dawn/dawncpp.h>
#include <dawn_native/DawnNative.h>
#include <shaderc/shaderc.hpp>
#include "common/Constants.h"
#include "utils/BackendBinding.h"
#include "utils/ComboRenderPipelineDescriptor.h"

#include <array>
#include <cstring>

ContextDawn::ContextDawn()
    : mWindow(nullptr),
      instance(),
      queue(nullptr),
      swapchain(nullptr),
      mCommandEncoder(nullptr),
      mRenderPass(nullptr),
      mRenderPassDescriptor({}),
      mBackbuffer(nullptr),
      mSceneRenderTargetView(nullptr),
      mSceneDepthStencilView(nullptr),
      mPipeline(nullptr),
      mBindGroup(nullptr),
      mPreferredSwapChainFormat(dawn::TextureFormat::R8G8B8A8Unorm),
      device(nullptr),
      mEnableMSAA(false)
{
}

ContextDawn::~ContextDawn()
{
    mSceneRenderTargetView   = nullptr;
    mSceneDepthStencilView   = nullptr;
    mBackbuffer              = nullptr;
    mPipeline                = nullptr;
    mBindGroup               = nullptr;
    lightWorldPositionBuffer = nullptr;
    lightBuffer              = nullptr;
    fogBuffer                = nullptr;
    mCommandEncoder          = nullptr;
    mCommandBuffers.clear();
    mRenderPass              = nullptr;
    mRenderPassDescriptor    = {};
    groupLayoutGeneral       = nullptr;
    bindGroupGeneral         = nullptr;
    groupLayoutWorld         = nullptr;
    bindGroupWorld           = nullptr;
    swapchain                = nullptr;
    queue                    = nullptr;
    device                   = nullptr;
}

bool ContextDawn::createContext(BACKENDTYPE backend, bool enableMSAA)
{
    dawn_native::BackendType backendType = dawn_native::BackendType::Null;

    switch (backend)
    {
        case BACKENDTYPE::BACKENDTYPEDAWND3D12:
        {
            backendType = dawn_native::BackendType::D3D12;
            break;
        }
        case BACKENDTYPE::BACKENDTYPEDAWNVULKAN:
        {
            backendType = dawn_native::BackendType::Vulkan;
            break;
        }
        case BACKENDTYPE::BACKENDTYPEDAWNMETAL:
        {
            backendType = dawn_native::BackendType::Metal;
            break;
        }
        case BACKENDTYPE::BACKENDTYPEOPENGL:
        {
            backendType = dawn_native::BackendType::OpenGL;
            break;
        }
        default:
        {
            std::cerr << "Backend type can not reached." << std::endl;
        }
    }

    mEnableMSAA = enableMSAA;

    // initialise GLFW
    if (!glfwInit())
    {
        std::cout << "Failed to initialise GLFW" << std::endl;
        return false;
    }

    utils::SetupGLFWWindowHintsForBackend(backendType);
    // set full screen
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

    GLFWmonitor *pMonitor   = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(pMonitor);
    mClientWidth            = mode->width;
    mClientHeight           = mode->height;
    // If we show the window bar on the top, max width and height will be 1916 x 1053.
    // Minus the height of title bar, or dawn vulkan backend cannot work.
    mClientHeight -= 20;

    mWindow = glfwCreateWindow(mClientWidth, mClientHeight, "Aquarium", NULL, NULL);
    if (mWindow == NULL)
    {
        std::cout << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        return false;
    }
    //glfwHideWindow(mWindow);

    instance = std::make_unique<dawn_native::Instance>();
    utils::DiscoverAdapter(instance.get(), mWindow, backendType);

    // Get an adapter for the backend to use, and create the device.
    dawn_native::Adapter backendAdapter;
    {
        for (auto &adapter : instance->GetAdapters())
        {
            if (adapter.GetBackendType() == backendType)
            {
                backendAdapter = adapter;
                break;
            }
        }
    }

    DawnDevice backendDevice   = backendAdapter.CreateDevice();
    DawnProcTable backendProcs = dawn_native::GetProcs();

    utils::BackendBinding *binding = utils::CreateBinding(backendType, mWindow, backendDevice);
    if (binding == nullptr)
    {
        return false;
    }

    dawnSetProcs(&backendProcs);
    device = dawn::Device::Acquire(backendDevice);

    queue = device.CreateQueue();
    dawn::SwapChainDescriptor swapChainDesc;
    swapChainDesc.implementation = binding->GetSwapChainImplementation();
    swapchain                    = device.CreateSwapChain(&swapChainDesc);

    mPreferredSwapChainFormat =
        static_cast<dawn::TextureFormat>(binding->GetPreferredSwapChainTextureFormat());
    swapchain.Configure(mPreferredSwapChainFormat, dawn::TextureUsageBit::OutputAttachment,
                        mClientWidth, mClientHeight);

    // When MSAA is enabled, we create an intermediate multisampled texture to render the scene to.
    if (mEnableMSAA)
    {
        dawn::TextureDescriptor descriptor;
        descriptor.dimension       = dawn::TextureDimension::e2D;
        descriptor.size.width      = mClientWidth;
        descriptor.size.height     = mClientHeight;
        descriptor.size.depth      = 1;
        descriptor.arrayLayerCount = 1;
        descriptor.sampleCount     = 4;
        descriptor.format          = mPreferredSwapChainFormat;
        descriptor.mipLevelCount   = 1;
        descriptor.usage           = dawn::TextureUsageBit::OutputAttachment;

        mSceneRenderTargetView = device.CreateTexture(&descriptor).CreateDefaultView();
    }

    mSceneDepthStencilView = createDepthStencilView();

    return true;
}

Texture *ContextDawn::createTexture(std::string name, std::string url)
{
    Texture *texture = new TextureDawn(this, name, url);
    texture->loadTexture();
    return texture;
}

Texture *ContextDawn::createTexture(std::string name, const std::vector<std::string> &urls)
{
    Texture *texture = new TextureDawn(this, name, urls);
    texture->loadTexture();
    return texture;
}

dawn::Texture ContextDawn::createTexture(const dawn::TextureDescriptor & descriptor) const
{
    return device.CreateTexture(&descriptor);
}

dawn::Sampler ContextDawn::createSampler(const dawn::SamplerDescriptor & descriptor) const
{
    return device.CreateSampler(&descriptor);
}

dawn::Buffer ContextDawn::createBufferFromData(const void * pixels, int size, dawn::BufferUsageBit usage) const
{
    return utils::CreateBufferFromData(device, pixels, size, usage);
}

dawn::BufferCopyView ContextDawn::createBufferCopyView(const dawn::Buffer& buffer,
    uint32_t offset,
    uint32_t rowPitch,
    uint32_t imageHeight) const {

    return utils::CreateBufferCopyView(buffer, offset, rowPitch, imageHeight);
}

dawn::TextureCopyView ContextDawn::createTextureCopyView(dawn::Texture texture,
                                                         uint32_t level,
                                                         uint32_t slice,
                                                         dawn::Origin3D origin)
{

    return utils::CreateTextureCopyView(texture, level, slice, origin);
}

dawn::CommandBuffer ContextDawn::copyBufferToTexture(const dawn::BufferCopyView &bufferCopyView, const dawn::TextureCopyView &textureCopyView, const dawn::Extent3D& ext3D) const
{
    dawn::CommandEncoder encoder = device.CreateCommandEncoder();
    encoder.CopyBufferToTexture(&bufferCopyView, &textureCopyView, &ext3D);
    dawn::CommandBuffer copy = encoder.Finish();
    return copy;
}

dawn::ShaderModule ContextDawn::createShaderModule(dawn::ShaderStage stage,
                                                   const std::string &str) const
{
    return utils::CreateShaderModule(device, stage, str.c_str());
}

dawn::BindGroupLayout ContextDawn::MakeBindGroupLayout(
    std::initializer_list<dawn::BindGroupLayoutBinding> bindingsInitializer) const {

    return utils::MakeBindGroupLayout(device, bindingsInitializer);
}

dawn::PipelineLayout ContextDawn::MakeBasicPipelineLayout(
    std::vector<dawn::BindGroupLayout> bindingsInitializer) const {
    dawn::PipelineLayoutDescriptor descriptor;

    descriptor.bindGroupLayoutCount = static_cast<uint32_t>(bindingsInitializer.size());
    descriptor.bindGroupLayouts = bindingsInitializer.data();
    
    return device.CreatePipelineLayout(&descriptor);
}

dawn::RenderPipeline ContextDawn::createRenderPipeline(
    dawn::PipelineLayout pipelineLayout,
    ProgramDawn *programDawn,
    dawn::VertexInputDescriptor &vertexInputDescriptor,
    bool enableBlend) const
{
    const dawn::ShaderModule &vsModule = programDawn->getVSModule();
    const dawn::ShaderModule &fsModule = programDawn->getFSModule();

    dawn::PipelineStageDescriptor cVertexStage;
    cVertexStage.entryPoint = "main";
    cVertexStage.module     = vsModule;

    dawn::PipelineStageDescriptor cFragmentStage;
    cFragmentStage.entryPoint = "main";
    cFragmentStage.module     = fsModule;

    dawn::BlendDescriptor blendDescriptor;
    blendDescriptor.operation = dawn::BlendOperation::Add;
    if (enableBlend)
    {
        blendDescriptor.srcFactor = dawn::BlendFactor::SrcAlpha;
        blendDescriptor.dstFactor = dawn::BlendFactor::OneMinusSrcAlpha;
    }
    else
    {
        blendDescriptor.srcFactor = dawn::BlendFactor::One;
        blendDescriptor.dstFactor = dawn::BlendFactor::Zero;
    }

    dawn::ColorStateDescriptor ColorStateDescriptor;
    ColorStateDescriptor.colorBlend     = blendDescriptor;
    ColorStateDescriptor.alphaBlend     = blendDescriptor;
    ColorStateDescriptor.writeMask      = dawn::ColorWriteMask::All;

    dawn::RasterizationStateDescriptor rasterizationState;
    rasterizationState.nextInChain         = nullptr;
    rasterizationState.frontFace           = dawn::FrontFace::CCW;
    rasterizationState.cullMode            = dawn::CullMode::Front;
    rasterizationState.depthBias           = 0;
    rasterizationState.depthBiasSlopeScale = 0.0;
    rasterizationState.depthBiasClamp      = 0.0;

    // test
    utils::ComboRenderPipelineDescriptor descriptor(device);
    descriptor.layout                               = pipelineLayout;
    descriptor.cVertexStage.module                  = vsModule;
    descriptor.cFragmentStage.module                = fsModule;
    descriptor.vertexInput                          = &vertexInputDescriptor;
    descriptor.depthStencilState                    = &descriptor.cDepthStencilState;
    descriptor.cDepthStencilState.format            = dawn::TextureFormat::D32FloatS8Uint;
    descriptor.cColorStates[0]                      = &ColorStateDescriptor;
    descriptor.cColorStates[0]->format              = mPreferredSwapChainFormat;
    descriptor.cDepthStencilState.depthWriteEnabled = true;
    descriptor.cDepthStencilState.depthCompare      = dawn::CompareFunction::Less;
    descriptor.primitiveTopology                    = dawn::PrimitiveTopology::TriangleList;
    descriptor.sampleCount                          = mEnableMSAA ? 4 : 1;
    descriptor.rasterizationState                   = &rasterizationState;

    dawn::RenderPipeline pipeline = device.CreateRenderPipeline(&descriptor);

    return pipeline;
}

dawn::TextureView ContextDawn::createDepthStencilView() const
{
    dawn::TextureDescriptor descriptor;
    descriptor.dimension       = dawn::TextureDimension::e2D;
    descriptor.size.width      = mClientWidth;
    descriptor.size.height     = mClientHeight;
    descriptor.size.depth      = 1;
    descriptor.arrayLayerCount = 1;
    descriptor.sampleCount     = mEnableMSAA ? 4 : 1;
    descriptor.format          = dawn::TextureFormat::D32FloatS8Uint;
    descriptor.mipLevelCount   = 1;
    descriptor.usage           = dawn::TextureUsageBit::OutputAttachment;
    auto depthStencilTexture   = device.CreateTexture(&descriptor);
    return depthStencilTexture.CreateDefaultView();
}

dawn::Buffer ContextDawn::createBuffer(uint32_t size, dawn::BufferUsageBit bit) const
{
    dawn::BufferDescriptor descriptor;
    descriptor.size = size;
    descriptor.usage = bit;

    dawn::Buffer buffer = device.CreateBuffer(&descriptor);
    return buffer;
}

void ContextDawn::setBufferData(const dawn::Buffer& buffer, uint32_t start, uint32_t size, const void* pixels) const
{
    buffer.SetSubData(start, size, reinterpret_cast<const uint8_t*>(pixels));
}

dawn::BindGroup ContextDawn::makeBindGroup(
    const dawn::BindGroupLayout &layout,
    std::initializer_list<utils::BindingInitializationHelper> bindingsInitializer) const
{
    return utils::MakeBindGroup(device, layout, bindingsInitializer);
}

void ContextDawn::initGeneralResources(Aquarium* aquarium)
{
    // initilize general uniform buffers
    groupLayoutGeneral = MakeBindGroupLayout({
        { 0, dawn::ShaderStageBit::Fragment, dawn::BindingType::UniformBuffer },
        { 1, dawn::ShaderStageBit::Fragment, dawn::BindingType::UniformBuffer },
    });

    lightBuffer = createBufferFromData(&aquarium->lightUniforms, sizeof(aquarium->lightUniforms), dawn::BufferUsageBit::TransferDst | dawn::BufferUsageBit::Uniform);
    fogBuffer = createBufferFromData(&aquarium->fogUniforms, sizeof(aquarium->fogUniforms), dawn::BufferUsageBit::TransferDst | dawn::BufferUsageBit::Uniform);

    bindGroupGeneral = makeBindGroup(groupLayoutGeneral, {
        { 0, lightBuffer, 0, sizeof(aquarium->lightUniforms) },
        { 1, fogBuffer , 0, sizeof(aquarium->fogUniforms) }
    });

    setBufferData(lightBuffer, 0, sizeof(LightUniforms), &aquarium->lightUniforms);
    setBufferData(fogBuffer, 0, sizeof(FogUniforms), &aquarium->fogUniforms);

    // initilize world uniform buffers
    groupLayoutWorld = MakeBindGroupLayout({ 
        { 0, dawn::ShaderStageBit::Vertex, dawn::BindingType::UniformBuffer },
    });

    lightWorldPositionBuffer = createBufferFromData(&aquarium->lightWorldPositionUniform, sizeof(aquarium->lightWorldPositionUniform), dawn::BufferUsageBit::TransferDst | dawn::BufferUsageBit::Uniform);
    
    bindGroupWorld = makeBindGroup(groupLayoutWorld, {
        {0, lightWorldPositionBuffer, 0, sizeof(aquarium->lightWorldPositionUniform) },
    });

    setBufferData(lightWorldPositionBuffer, 0, sizeof(LightWorldPositionUniform), &aquarium->lightWorldPositionUniform);
}

void ContextDawn::updateWorldlUniforms(Aquarium* aquarium)
{
    setBufferData(lightWorldPositionBuffer, 0, sizeof(LightWorldPositionUniform), &aquarium->lightWorldPositionUniform);
}

Buffer *ContextDawn::createBuffer(int numComponents, std::vector<float> &buf, bool isIndex)
{
    Buffer *buffer = new BufferDawn(this, static_cast<int>(buf.size()), numComponents, buf, isIndex);
    return buffer;
}

Buffer *ContextDawn::createBuffer(int numComponents, std::vector<unsigned short> &buf, bool isIndex)
{
    Buffer *buffer = new BufferDawn(this, static_cast<int>(buf.size()), numComponents, buf, isIndex);
    return buffer;
}

Program *ContextDawn::createProgram(std::string vId, std::string fId)
{
    ProgramDawn* program = new ProgramDawn(this, vId, fId);
    program->loadProgram();

    return program;
}

void ContextDawn::setWindowTitle(const std::string &text)
{
    glfwSetWindowTitle(mWindow, text.c_str());
}

bool ContextDawn::ShouldQuit()
{
    return glfwWindowShouldClose(mWindow);
}

void ContextDawn::KeyBoardQuit()
{
    if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(mWindow, GL_TRUE);
}

// Submit commands of the frame
void ContextDawn::DoFlush()
{

    mRenderPass.EndPass();
    dawn::CommandBuffer cmd = mCommandEncoder.Finish();
    queue.Submit(1, &cmd);

    swapchain.Present(mBackbuffer);

    glfwPollEvents();
}

void ContextDawn::FlushInit()
{
    queue.Submit(mCommandBuffers.size(), mCommandBuffers.data());
}

void ContextDawn::Terminate()
{
    glfwTerminate();
}

void ContextDawn::showWindow()
{
    glfwShowWindow(mWindow);
}

void ContextDawn::preFrame()
{
    mCommandEncoder = device.CreateCommandEncoder();
    mBackbuffer = swapchain.GetNextTexture();

    if (mEnableMSAA)
    {
        // If MSAA is enabled, we render to a multisampled texture and then resolve to the backbuffer
        mRenderPassDescriptor = utils::ComboRenderPassDescriptor({mSceneRenderTargetView},
                                                                 mSceneDepthStencilView);
        mRenderPassDescriptor.cColorAttachmentsInfoPtr[0]->resolveTarget =
            mBackbuffer.CreateDefaultView();
    }
    else
    {
        // When MSAA is off, we render directly to the backbuffer
        mRenderPassDescriptor = utils::ComboRenderPassDescriptor({mBackbuffer.CreateDefaultView()},
                                                                 mSceneDepthStencilView);
    }

    mRenderPass = mCommandEncoder.BeginRenderPass(&mRenderPassDescriptor);
}

Model * ContextDawn::createModel(Aquarium* aquarium, MODELGROUP type, MODELNAME name, bool blend)
{
    Model *model;
    switch (type)
    {
    case MODELGROUP::FISH:
        model = new FishModelDawn(this, aquarium, type, name, blend);
        break;
    case MODELGROUP::FISHINSTANCEDDRAW:
        model = new FishModelInstancedDrawDawn(this, aquarium, type, name, blend);
        break;
    case MODELGROUP::GENERIC:
        model = new GenericModelDawn(this, aquarium, type, name, blend);
        break;
    case MODELGROUP::INNER:
        model = new InnerModelDawn(this, aquarium, type, name, blend);
        break;
    case MODELGROUP::SEAWEED:
        model = new SeaweedModelDawn(this, aquarium, type, name, blend);
        break;
    case MODELGROUP::OUTSIDE:
        model = new OutsideModelDawn(this, aquarium, type, name, blend);
        break;
    default:
        model = nullptr;
        std::cout << "can not create model type" << std::endl;
    }

    return model;
}

