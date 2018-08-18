#include "Shader.h"
#include <utils/Utils.h>

bgfx::ShaderHandle Shader::loadShader(const char* basePath, const char* _name)
{
    char filePath[512];

    const char* shaderPath = "shaders/dx9/";

    switch (bgfx::getRendererType())
    {
        case bgfx::RendererType::Direct3D11:
        case bgfx::RendererType::Direct3D12:
            shaderPath = "shaders/dx11/";
            break;

        case bgfx::RendererType::OpenGL:
            shaderPath = "shaders/glsl/";
            break;

        case bgfx::RendererType::Metal:
            shaderPath = "shaders/metal/";
            break;

        case bgfx::RendererType::OpenGLES:
            shaderPath = "shaders/essl/";
            break;

        default:
            break;
    }

    strcpy(filePath, basePath);
    strcat(filePath, shaderPath);
    strcat(filePath, _name);
    strcat(filePath, ".bin");

    return bgfx::createShader(Utils::loadFileToMemory(filePath));
}

bgfx::ProgramHandle Shader::loadProgram(const char* basePath, const char* _vsName, const char* _fsName)
{
    bgfx::ShaderHandle vsh = loadShader(basePath, _vsName);
    bgfx::ShaderHandle fsh = BGFX_INVALID_HANDLE;
    if (nullptr != _fsName)
    {
        fsh = loadShader(basePath, _fsName);
    }

    return bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);
}
