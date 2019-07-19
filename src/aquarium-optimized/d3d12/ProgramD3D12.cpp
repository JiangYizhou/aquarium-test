#include <cstring>
#include <fstream>

#include "ContextD3D12.h"
#include "ProgramD3D12.h"

ProgramD3D12::ProgramD3D12(ContextD3D12 *context, const std::string &vId, const std::string &fId)
    : Program(vId, fId), vertexShader(nullptr), pixelShader(nullptr), context(context)
{
}

ProgramD3D12::~ProgramD3D12() {}

void ProgramD3D12::loadProgram()
{
    std::ifstream VertexShaderStream(vId, std::ios::in);
    std::string VertexShaderCode((std::istreambuf_iterator<char>(VertexShaderStream)),
                                 std::istreambuf_iterator<char>());
    VertexShaderStream.close();

    // Read the Fragment Shader code from the file
    std::ifstream FragmentShaderStream(fId, std::ios::in);
    std::string FragmentShaderCode((std::istreambuf_iterator<char>(FragmentShaderStream)),
                                   std::istreambuf_iterator<char>());
    FragmentShaderStream.close();

    vertexShader = context->createShaderModule("VS", VertexShaderCode);
    pixelShader  = context->createShaderModule("PS", FragmentShaderCode);
}
