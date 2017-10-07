#include <bgfx/bgfx.h>

namespace Shader
{
    bgfx::ShaderHandle loadShader(const char* basePath, const char* _name);
    bgfx::ProgramHandle loadProgram(const char* basePath, const char* _vsName, const char* _fsName);
}
