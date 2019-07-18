#include <cstring>
#include <fstream>

#include "ContextD3D12.h"
#include "ProgramD3D12.h"

ProgramD3D12::ProgramD3D12(ContextD3D12 *context, std::string vId, std::string fId)
    : Program(vId, fId), mVertexShader(nullptr), mPixelShader(nullptr), mContext(context)
{
}

ProgramD3D12::~ProgramD3D12() {}

void ProgramD3D12::loadProgram()
{
    std::ifstream VertexShaderStream(mVId, std::ios::in);
    std::string VertexShaderCode((std::istreambuf_iterator<char>(VertexShaderStream)),
                                 std::istreambuf_iterator<char>());
    VertexShaderStream.close();

    // Read the Fragment Shader code from the file
    std::ifstream FragmentShaderStream(mFId, std::ios::in);
    std::string FragmentShaderCode((std::istreambuf_iterator<char>(FragmentShaderStream)),
                                   std::istreambuf_iterator<char>());
    FragmentShaderStream.close();

    mVertexShader = mContext->createShaderModule("VS", VertexShaderCode);
    mPixelShader  = mContext->createShaderModule("PS", FragmentShaderCode);
}
