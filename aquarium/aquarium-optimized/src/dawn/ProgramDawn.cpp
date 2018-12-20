#include "ProgramDawn.h"

#include "ContextDawn.h"
#include "../ASSERT.h"
#include <fstream>
#include <string.h>
#include <regex>

ProgramDawn::ProgramDawn(ContextDawn *context, string vId, string fId)
    : context(context), Program(vId, fId)
{
}

ProgramDawn::~ProgramDawn()
{
}

void ProgramDawn::loadProgram()
{
    ifstream VertexShaderStream(vId, ios::in);
    std::string VertexShaderCode((std::istreambuf_iterator<char>(VertexShaderStream)),
        std::istreambuf_iterator<char>());
    VertexShaderStream.close();

    // Read the Fragment Shader code from the file
    ifstream FragmentShaderStream(fId, ios::in);
    std::string FragmentShaderCode((std::istreambuf_iterator<char>(FragmentShaderStream)),
        std::istreambuf_iterator<char>());
    FragmentShaderStream.close();

#ifdef _WIN32
    FragmentShaderCode =
        std::regex_replace(FragmentShaderCode, std::regex(R"(^.*?// #noReflection\n)"), "");
    FragmentShaderCode =
        std::regex_replace(FragmentShaderCode, std::regex(R"(^.*?// #noNormalMap\n)"), "");
#else
    FragmentShaderCode =
        std::regex_replace(FragmentShaderCode, std::regex(R"(\n.*?// #noReflection)"), "");
    FragmentShaderCode =
        std::regex_replace(FragmentShaderCode, std::regex(R"(\n.*?// #noNormalMap)"), "");
#endif

    vsModule = context->createShaderModule(dawn::ShaderStage::Vertex, VertexShaderCode, vId);
    fsModule = context->createShaderModule(dawn::ShaderStage::Fragment, FragmentShaderCode, fId);
}
