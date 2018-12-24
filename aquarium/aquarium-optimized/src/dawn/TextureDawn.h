// TextureDawn.h: Wrap textures of Dawn.

#pragma once
#ifndef TEXTUREDAWN_H
#define TEXTUREDAWN_H

#include "../Texture.h"
#include <dawn/dawncpp.h>

class ContextDawn;

class TextureDawn : public Texture
{
  public:
    ~TextureDawn() override;
    TextureDawn(ContextDawn* context, std::string name, std::string url);
    TextureDawn(ContextDawn *context, std::string name, const std::vector<std::string> &urls);

    const dawn::Texture &getTextureId() const { return mTexture; }
    const dawn::Sampler &getSampler() const { return mSampler; }
    dawn::TextureDimension getTextureDimension() { return mTextureDimension; }
    dawn::TextureViewDimension getTextureViewDimension() { return mTextureViewDimension; }
    dawn::TextureView getTextureView() { return mTextureView; }

    void loadTexture() override;

  private:
    dawn::TextureDimension mTextureDimension;  // texture 2D or CubeMap
    dawn::TextureViewDimension mTextureViewDimension;
    dawn::Texture mTexture;
    dawn::Sampler mSampler;
    dawn::TextureFormat mFormat;
    dawn::TextureView mTextureView;
    std::vector<unsigned char *> pixelVec;
    std::vector<unsigned char *> resizedVec;
    ContextDawn *context;
};

#endif // !TEXTUREDAWN_H