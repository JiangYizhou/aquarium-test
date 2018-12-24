//
// Copyright (c) 2018 The WebGLNativePorts Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Texture.cpp: Use stb image loader to loading images from files.

#include "Texture.h"

#include <iostream>
#include <stdio.h>

#include "ASSERT.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_resize.h"

Texture::Texture(std::string name, const std::string &url)
    : mUrls(NULL),
    mWidth(0),
    mHeight(0),
    mName(name),
    mFlip(true)
{
    std::string urlpath = url;
    mUrls.push_back(urlpath);
}

// Force loading 3 channel images to 4 channel by stb becasue Dawn doesn't support 3 channel
// formats currently. The group is discussing on whether webgpu shoud support 3 channel format.
// https://github.com/gpuweb/gpuweb/issues/66#issuecomment-410021505
bool Texture::loadImage(const std::vector<std::string> &urls, std::vector<uint8_t *>* pixels)
{
    stbi_set_flip_vertically_on_load(mFlip);
    for (auto filename : urls) {
        uint8_t *pixel = stbi_load(filename.c_str(), &mWidth, &mHeight, 0, 4);
        if (pixel == 0)
        {
            std::cout << stderr << "Couldn't open input file" << filename << std::endl;
            return false;
        }
        pixels->push_back(pixel);
    }
    return true;
}

bool Texture::isPowerOf2(int value)
{
    return (value & (value - 1)) == 0;
}

// Free image data after upload to gpu
void Texture::DestoryImageData(std::vector<uint8_t*>& pixelVec)
{
    for (auto& pixels : pixelVec)
    {
        free(pixels);
        pixels = nullptr;
    }
}

void Texture::resizeImages(std::vector<uint8_t *>& input_pixels, int input_w, int input_h, int input_stride_in_bytes, std::vector<uint8_t *>& output_pixels, int output_w, int output_h, int output_stride_in_bytes, int num_channels)
{
    for (size_t i = 0; i < input_pixels.size(); ++i)
    {
        stbir_resize_uint8(input_pixels[i], input_w, input_h, input_stride_in_bytes,
            input_pixels[i], output_w, output_h, output_stride_in_bytes, num_channels);
    }
}
