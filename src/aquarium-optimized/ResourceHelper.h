#pragma once
#include <string>
#include <vector>

class ResourceHelper
{
  public:
    ResourceHelper() {}
    ResourceHelper(const std::string &mBackendName, const std::string &mShaderVersion);
    void getSkyBoxUrls(std::vector<std::string> *skyUrls) const;
    const std::string &getPropPlacementPath() const { return mPropPlacementPath; }
    const std::string &getImagePath() const { return mImagePath; }
    std::string getModelPath(const std::string &modelName) const;
    std::string getProgramPath() const;

  private:
    std::string mPath;
    std::string mImagePath;
    std::string mProgramPath;
    std::string mPropPlacementPath;
    std::string mBackendName;
    std::string mShaderVersion;
};
