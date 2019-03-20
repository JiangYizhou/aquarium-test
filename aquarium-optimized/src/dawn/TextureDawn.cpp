//
// Copyright (c) 2019 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// TextureDawn.cpp: Wrap textures of Dawn. Load image files and wrap into a Dawn texture.

#include "TextureDawn.h"

#include <algorithm>
#include <cmath>

#include "ContextDawn.h"

#include "../ASSERT.h"

TextureDawn::~TextureDawn() {

    DestoryImageData(mPixelVec);
    DestoryImageData(mResizedVec);
}

TextureDawn::TextureDawn(ContextDawn *context, std::string name, std::string url)
    : Texture(name, url, true),
      mTextureDimension(dawn::TextureDimension::e2D),
      mTextureViewDimension(dawn::TextureViewDimension::e2D),
      mTexture(nullptr),
      mSampler(nullptr),
      mFormat(dawn::TextureFormat::R8G8B8A8Unorm),
      mTextureView(nullptr),
      context(context)
{
}

TextureDawn::TextureDawn(ContextDawn *context,
                         std::string name,
                         const std::vector<std::string> &urls)
    : Texture(name, urls, false),
      mTextureDimension(dawn::TextureDimension::e2D),
      mTextureViewDimension(dawn::TextureViewDimension::Cube),
      mFormat(dawn::TextureFormat::R8G8B8A8Unorm),
      context(context)
{
}

void TextureDawn::loadTexture()
{
    dawn::SamplerDescriptor samplerDesc;
    const int kPadding = 256;
    loadImage(mUrls, &mPixelVec);

    if (mTextureViewDimension == dawn::TextureViewDimension::Cube)
    {
        dawn::TextureDescriptor descriptor;
        descriptor.dimension = mTextureDimension;
        descriptor.size.width = mWidth;
        descriptor.size.height = mHeight;
        descriptor.size.depth = 1;
        descriptor.arraySize = 6;
        descriptor.sampleCount = 1;
        descriptor.format = mFormat;
        descriptor.levelCount = 1;
        descriptor.usage = dawn::TextureUsageBit::TransferDst | dawn::TextureUsageBit::Sampled;
        mTexture = context->createTexture(descriptor);

        for (unsigned int i = 0; i < 6; i++)
        {
            dawn::Buffer stagingBuffer = context->createBufferFromData(mPixelVec[i], mWidth * mHeight * 4, dawn::BufferUsageBit::TransferSrc);
            dawn::BufferCopyView bufferCopyView = context->createBufferCopyView(stagingBuffer, 0, mWidth * 4, mHeight);
            dawn::TextureCopyView textureCopyView = context->createTextureCopyView(mTexture, 0, i, { 0, 0, 0 });
            dawn::Extent3D copySize = { static_cast<uint32_t>(mWidth), static_cast<uint32_t>(mHeight), 1 };
            dawn::CommandBuffer cmd = context->copyBufferToTexture(bufferCopyView, textureCopyView, copySize);
            context->submit(1, cmd);
        }

        dawn::TextureViewDescriptor viewDescriptor;
        viewDescriptor.nextInChain = nullptr;
        viewDescriptor.dimension = dawn::TextureViewDimension::Cube;
        viewDescriptor.format = mFormat;
        viewDescriptor.baseMipLevel = 0;
        viewDescriptor.levelCount = 1;
        viewDescriptor.baseArrayLayer = 0;
        viewDescriptor.layerCount = 6;

        mTextureView = mTexture.CreateTextureView(&viewDescriptor);

        samplerDesc.addressModeU = dawn::AddressMode::ClampToEdge;
        samplerDesc.addressModeV = dawn::AddressMode::ClampToEdge;
        samplerDesc.addressModeW = dawn::AddressMode::ClampToEdge;
        samplerDesc.minFilter = dawn::FilterMode::Linear;
        samplerDesc.magFilter = dawn::FilterMode::Linear;
        samplerDesc.mipmapFilter = dawn::FilterMode::Nearest;
        samplerDesc.lodMinClamp  = 0.0f;
        samplerDesc.lodMaxClamp  = 1000.0f;
        samplerDesc.compareFunction = dawn::CompareFunction::Never;
        samplerDesc.borderColor     = dawn::BorderColor::TransparentBlack;

        mSampler = context->createSampler(samplerDesc);
    }
    else  // dawn::TextureViewDimension::e2D
    {
        int resizedWidth;
        if (mWidth % kPadding == 0)
        {
            resizedWidth = mWidth;
        }
        else
        {
            resizedWidth = (mWidth / 256 + 1) * 256;
        }
        generateMipmap(mPixelVec[0], mWidth, mHeight, 0, mResizedVec, resizedWidth, mHeight, 0, 4);

        dawn::TextureDescriptor descriptor;
        descriptor.dimension = mTextureDimension;
        descriptor.size.width  = resizedWidth;
        descriptor.size.height = mHeight;
        descriptor.size.depth = 1;
        descriptor.arraySize = 1;
        descriptor.sampleCount = 1;
        descriptor.format = mFormat;
        descriptor.levelCount =
            static_cast<uint32_t>(std::floor(static_cast<float>(std::log2(std::max(mWidth, mHeight))))) + 1;
        descriptor.usage = dawn::TextureUsageBit::TransferDst | dawn::TextureUsageBit::Sampled;
        mTexture = context->createTexture(descriptor);

        for (unsigned int i = 0; i < descriptor.levelCount; ++i)
        {
            int height                 = mHeight >> i;
            int width                  = resizedWidth >> i;
            if (width ==0 || height == 0)
                break;
            dawn::Buffer stagingBuffer = context->createBufferFromData(mResizedVec[i], resizedWidth * height * 4, dawn::BufferUsageBit::TransferSrc);
            dawn::BufferCopyView bufferCopyView = context->createBufferCopyView(stagingBuffer, 0, resizedWidth * 4, height);
            dawn::TextureCopyView textureCopyView = context->createTextureCopyView(mTexture, i, 0, { 0, 0, 0 });
            dawn::Extent3D copySize = {static_cast<uint32_t>(width),
                                       static_cast<uint32_t>(height),
                                       1};
            dawn::CommandBuffer cmd = context->copyBufferToTexture(bufferCopyView, textureCopyView, copySize);

            context->submit(1, cmd);
        }

        dawn::TextureViewDescriptor viewDescriptor;
        viewDescriptor.nextInChain = nullptr;
        viewDescriptor.dimension = dawn::TextureViewDimension::e2D;
        viewDescriptor.format = mFormat;
        viewDescriptor.baseMipLevel = 0;
        viewDescriptor.levelCount =
            static_cast<uint32_t>(std::floor(static_cast<float>(std::log2(std::max(mWidth, mHeight))))) + 1;
        viewDescriptor.baseArrayLayer = 0;
        viewDescriptor.layerCount = 1;

        mTextureView = mTexture.CreateTextureView(&viewDescriptor);

        samplerDesc.addressModeU = dawn::AddressMode::ClampToEdge;
        samplerDesc.addressModeV = dawn::AddressMode::ClampToEdge;
        samplerDesc.addressModeW = dawn::AddressMode::ClampToEdge;
        samplerDesc.minFilter = dawn::FilterMode::Linear;
        samplerDesc.magFilter = dawn::FilterMode::Linear;
        samplerDesc.lodMinClamp  = 0.0f;
        samplerDesc.lodMaxClamp  = 1000.0f;
        samplerDesc.compareFunction = dawn::CompareFunction::Never;
        samplerDesc.borderColor     = dawn::BorderColor::TransparentBlack;

        if (isPowerOf2(mWidth) && isPowerOf2(mHeight))
        {
            samplerDesc.mipmapFilter = dawn::FilterMode::Linear;
        }
        else
        {
            samplerDesc.mipmapFilter = dawn::FilterMode::Nearest;
        }

        mSampler = context->createSampler(samplerDesc);
    }

    // TODO(yizhou): check if the pixel destory should delay or fence
}

