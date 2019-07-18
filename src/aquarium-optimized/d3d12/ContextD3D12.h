#pragma once
//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// ContextD3D12.h : Defines the accessing to graphics API of D3D12.

#pragma once
#ifndef CONTEXTD3D12_H
#define CONTEXTD3D12_H

#include "../Context.h"

#include "GLFW/glfw3.h"

#include "stdafx.h"
using Microsoft::WRL::ComPtr;

enum BACKENDTYPE : short;

constexpr int cbvsrvCount = 87;

class ContextD3D12 : public Context
{
  public:
    ContextD3D12(BACKENDTYPE backendType);
    ~ContextD3D12();
    bool initialize(
        BACKENDTYPE backend,
        const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset) override;
    void setWindowTitle(const std::string &text) override;
    bool ShouldQuit() override;
    void KeyBoardQuit() override;
    void DoFlush() override;
    void Terminate() override;
    void showWindow() override;
    void showFPS(const FPSTimer &fpsTimer) override;
    void destoryImgUI() override;

    void FlushInit() override;
    void preFrame() override;

    Model *createModel(Aquarium *aquarium, MODELGROUP type, MODELNAME name, bool blend) override;
    Buffer *createBuffer(int numComponents, std::vector<float> &buffer, bool isIndex) override;
    Buffer *createBuffer(int numComponents,
                         std::vector<unsigned short> &buffer,
                         bool isIndex) override;

    Program *createProgram(std::string vId, std::string fId) override;

    Texture *createTexture(std::string name, std::string url) override;
    Texture *createTexture(std::string name, const std::vector<std::string> &urls) override;

    void initGeneralResources(Aquarium *aquarium) override;
    void updateWorldlUniforms(Aquarium *aquarium) override;

    ComPtr<ID3DBlob> createShaderModule(const std::string &type, const std::string &shader);
    void createCommittedResource(const D3D12_HEAP_PROPERTIES &properties,
                                 const D3D12_RESOURCE_DESC &textureDesc,
                                 D3D12_RESOURCE_STATES state,
                                 ComPtr<ID3D12Resource> &);
    void updateSubresources(ID3D12GraphicsCommandList *pCmdList,
                            ID3D12Resource *pDestinationResource,
                            ID3D12Resource *pIntermediate,
                            UINT64 IntermediateOffset,
                            UINT FirstSubresource,
                            UINT NumSubresources,
                            D3D12_SUBRESOURCE_DATA *pSrcData);
    void createShaderResourceView(ID3D12Resource *pResource,
                                  const D3D12_SHADER_RESOURCE_VIEW_DESC *pDesc,
                                  D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor);
    void executeCommandLists(UINT NumCommandLists, ID3D12CommandList *const *ppCommandLists);

    void createCommandList(ID3D12PipelineState *pInitialState,
                           ComPtr<ID3D12GraphicsCommandList> &commandList);

    ComPtr<ID3D12Resource> createDefaultBuffer(const void *initData,
                                               UINT64 byteSize,
                                               ComPtr<ID3D12Resource> &uploadBuffer) const;
    ComPtr<ID3D12Resource> createUploadBuffer(const void *initData, UINT64 byteSize) const;
    void createRootSignature(const D3D12_VERSIONED_ROOT_SIGNATURE_DESC *pRootSignatureDesc,
                             ComPtr<ID3D12RootSignature> &mRootSignature) const;
    void createGraphicsPipelineState(
        const std::vector<D3D12_INPUT_ELEMENT_DESC> &mInputElementDescs,
        const ComPtr<ID3D12RootSignature> &rootSignature,
        const ComPtr<ID3DBlob> &vertexShader,
        const ComPtr<ID3DBlob> &pixelShader,
        ComPtr<ID3D12PipelineState> &mPipelineState,
        bool enableBlend) const;
    void createSampler(D3D12_SAMPLER_DESC *pDesc, ComPtr<ID3D12DescriptorHeap> &samplerHeap) const;
    void buildSrvDescriptor(const ComPtr<ID3D12Resource> &resource,
                            const D3D12_SHADER_RESOURCE_VIEW_DESC &srvDesc,
                            OUT D3D12_GPU_DESCRIPTOR_HANDLE *hGpuDescriptor);
    void buildCbvDescriptor(const D3D12_CONSTANT_BUFFER_VIEW_DESC &cbvDesc,
                            OUT D3D12_GPU_DESCRIPTOR_HANDLE *hGpuDescriptor);
    UINT CalcConstantBufferByteSize(UINT byteSize);
    void createTexture(const D3D12_RESOURCE_DESC &textureDesc,
                       OUT ComPtr<ID3D12Resource> &m_texture,
                       OUT ComPtr<ID3D12Resource> &textureUploadHeap,
                       std::vector<UINT8 *> &texture,
                       int TextureWidth,
                       int TextureHeight,
                       int TexturePixelSize,
                       int mipLevels,
                       int arraySize);

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

    CD3DX12_DESCRIPTOR_RANGE1 rangeGeneral[2];
    CD3DX12_ROOT_PARAMETER1 rootParameterGeneral;
    CD3DX12_ROOT_PARAMETER1 rootParameterWorld;
    D3D12_GPU_DESCRIPTOR_HANDLE fogGPUHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE lightGPUHandle;
    D3D12_CONSTANT_BUFFER_VIEW_DESC lightWorldPositionView;
    CD3DX12_CPU_DESCRIPTOR_HANDLE cbvsrvCPUHandle;
    CD3DX12_GPU_DESCRIPTOR_HANDLE cbvsrvGPUHandle;

    std::vector<CD3DX12_STATIC_SAMPLER_DESC> staticSamplers;

  private:
    bool GetHardwareAdapter(
        IDXGIFactory2 *pFactory,
        IDXGIAdapter1 **ppAdapter,
        const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset);
    void WaitForPreviousFrame();
    void createDepthStencilView();
    void stateTransition(ComPtr<ID3D12Resource> &resource,
                         D3D12_RESOURCE_STATES preState,
                         D3D12_RESOURCE_STATES transferState) const;
    void initAvailableToggleBitset(BACKENDTYPE backendType) override;

    GLFWwindow *mWindow;
    ComPtr<ID3D12Device> mDevice;
    ComPtr<ID3D12CommandQueue> mCommandQueue;
    ComPtr<IDXGISwapChain3> mSwapChain;
    DXGI_FORMAT mPreferredSwapChainFormat;
    UINT mCompileFlags;

    static const UINT mFrameCount = 3;
    UINT mFrameIndex;
    UINT mBufferSerias[mFrameCount];
    ComPtr<ID3D12CommandAllocator> mCommandAllocators[mFrameCount];

    ComPtr<ID3D12DescriptorHeap> mRtvHeap;
    ComPtr<ID3D12DescriptorHeap> mDsvHeap;
    ComPtr<ID3D12DescriptorHeap> mCbvsrvHeap;
    UINT mRtvDescriptorSize;
    UINT mCbvsrvDescriptorSize;
    ComPtr<ID3D12Resource> mRenderTargets[mFrameCount];
    ComPtr<ID3D12Resource> mDepthStencil;

    ComPtr<ID3D12Fence> mFence;
    UINT64 mFenceValue;
    HANDLE mFenceEvent;

    D3D12_FEATURE_DATA_ROOT_SIGNATURE mRootSignature;

    CD3DX12_VIEWPORT mViewport;
    CD3DX12_RECT mScissorRect;

    // General Resources
    ComPtr<ID3D12Resource> mLightWorldPositionBuffer;

    D3D12_CONSTANT_BUFFER_VIEW_DESC mLightView;
    ComPtr<ID3D12Resource> mLightBuffer;
    ComPtr<ID3D12Resource> mLightUploadBuffer;

    D3D12_CONSTANT_BUFFER_VIEW_DESC mFogView;
    ComPtr<ID3D12Resource> mFogBuffer;
    ComPtr<ID3D12Resource> mFogUploadBuffer;

    ComPtr<ID3D12Resource> mSceneRenderTargetTexture;
    D3D12_RENDER_TARGET_VIEW_DESC mSceneRenderTargetView;

    bool mEnableMSAA;
    std::string mRenderer;
};

#endif
