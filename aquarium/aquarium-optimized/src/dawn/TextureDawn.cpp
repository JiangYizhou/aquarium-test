// TextureDawn.cpp: Wrap textures of Dawn. Load image files and wrap into a Dawn texture.

#include "TextureDawn.h"

#include <algorithm>
#include <cmath>

#include "ContextDawn.h"

#include "../ASSERT.h"

TextureDawn::~TextureDawn() {

    DestoryImageData(pixelVec);
    DestoryImageData(resizedVec);
}

TextureDawn::TextureDawn(ContextDawn * context, std::string name, std::string url)
    : context(context), mTextureDimension(dawn::TextureDimension::e2D), mTextureViewDimension(dawn::TextureViewDimension::e2D), mFormat(dawn::TextureFormat::R8G8B8A8Unorm), Texture(name, url)
{
    resizedVec.resize(1);
}

TextureDawn::TextureDawn(ContextDawn * context, std::string name, const std::vector<std::string>& urls)
    : context(context), mTextureDimension(dawn::TextureDimension::e2D), mTextureViewDimension(dawn::TextureViewDimension::Cube), mFormat(dawn::TextureFormat::R8G8B8A8Unorm), Texture(name, urls)
{
}

void TextureDawn::loadTexture()
{
    dawn::SamplerDescriptor samplerDesc;
    const int kPadding = 256;
    loadImage(mUrls, &pixelVec);

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
            dawn::Buffer stagingBuffer = context->createBufferFromData(pixelVec[i], mWidth * mHeight * 4, dawn::BufferUsageBit::TransferSrc);
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

        mSampler = context->createSampler(samplerDesc);
    }
    else  // dawn::TextureViewDimension::e2D
    {
        int resizedWidth;
        bool resized = false;
        if (mWidth % kPadding == 0)
        {
            resizedWidth = mWidth;
        }
        else
        {
            resizedWidth = (mWidth / 256 + 1) * 256;
            resizedVec[0] = (unsigned char *) malloc (resizedWidth * mHeight * 4 * sizeof(char)) ;
            resizeImages(pixelVec[0], mWidth, mHeight, 0, resizedVec[0], resizedWidth, mHeight,
                         0, 4);
            resized = true;
        }
       
        dawn::TextureDescriptor descriptor;
        descriptor.dimension = mTextureDimension;
        descriptor.size.width  = resizedWidth;
        descriptor.size.height = mHeight;
        descriptor.size.depth = 1;
        descriptor.arraySize = 1;
        descriptor.sampleCount = 1;
        descriptor.format = mFormat;
        // static_cast<uint32_t>(std::floor(std::log2(std::max(mWidth, mHeight)))) + 1
        descriptor.levelCount = 1;
        descriptor.usage = dawn::TextureUsageBit::TransferDst | dawn::TextureUsageBit::Sampled;
        mTexture = context->createTexture(descriptor);

        // TODO(yizhou) : check the data size of pixels, jpeg image
        dawn::Buffer stagingBuffer = context->createBufferFromData(resized? resizedVec[0] : pixelVec[0], resizedWidth * mHeight * 4, dawn::BufferUsageBit::TransferSrc);
        dawn::BufferCopyView bufferCopyView = context->createBufferCopyView(stagingBuffer, 0, resizedWidth * 4, mHeight);
        dawn::TextureCopyView textureCopyView = context->createTextureCopyView(mTexture, 0, 0, { 0, 0, 0 });
        dawn::Extent3D copySize = { static_cast<uint32_t>(resizedWidth), static_cast<uint32_t>(mHeight), 1 };
        dawn::CommandBuffer cmd = context->copyBufferToTexture(bufferCopyView, textureCopyView, copySize);

        context->submit(1, cmd);

        dawn::TextureViewDescriptor viewDescriptor;
        viewDescriptor.nextInChain = nullptr;
        viewDescriptor.dimension = dawn::TextureViewDimension::e2D;
        viewDescriptor.format = mFormat;
        viewDescriptor.baseMipLevel = 0;
        // static_cast<uint32_t>(std::floor(std::log2(std::max(mWidth, mHeight)))) + 1
        viewDescriptor.levelCount = 1;
        viewDescriptor.baseArrayLayer = 0;
        viewDescriptor.layerCount = 1;

        mTextureView = mTexture.CreateTextureView(&viewDescriptor);

        samplerDesc.addressModeU = dawn::AddressMode::ClampToEdge;
        samplerDesc.addressModeV = dawn::AddressMode::ClampToEdge;
        samplerDesc.addressModeW = dawn::AddressMode::ClampToEdge;
        samplerDesc.minFilter = dawn::FilterMode::Linear;
        samplerDesc.magFilter = dawn::FilterMode::Linear;

        if (isPowerOf2(mWidth) && isPowerOf2(mHeight))
        {
            // TODO(yizhou) : generateMipmap
            samplerDesc.mipmapFilter = dawn::FilterMode::Linear;
        }
        else
        {
            samplerDesc.mipmapFilter = dawn::FilterMode::Nearest;
        }

        mSampler = context->createSampler(samplerDesc);
    }

    // TODO(yizhou): check if the pxiel destory should delay or fence
}

