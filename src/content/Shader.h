#pragma once

#include <bx/readerwriter.h>

namespace Shader
{
    static const bgfx::Memory* loadMem(bx::FileReaderI* _reader, const char* _filePath)
    {
        if (bx::open(_reader, _filePath))
        {
            uint32_t size = (uint32_t) bx::getSize(_reader);
            const bgfx::Memory* mem = bgfx::alloc(size + 1);
            bx::read(_reader, mem->data, size);
            bx::close(_reader);
            mem->data[mem->size - 1] = '\0';
            return mem;
        }

        DBG("Failed to load %s.", _filePath);
        return NULL;
    }

    static bgfx::ShaderHandle loadShader(bx::FileReaderI* _reader, const char* basePath, const char* _name)
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
                shaderPath = "shaders/gles/";
                break;

            default:
                break;
        }

        strcpy(filePath, basePath);
        strcat(filePath, shaderPath);
        strcat(filePath, _name);
        strcat(filePath, ".bin");

        return bgfx::createShader(loadMem(_reader, filePath));
    }

    bgfx::ShaderHandle loadShader(const char* basePath, const char* _name)
    {
        return loadShader(entry::getFileReader(), basePath, _name);
    }

    bgfx::ProgramHandle loadProgram(bx::FileReaderI* _reader, const char* basePath, const char* _vsName, const char* _fsName)
    {
        bgfx::ShaderHandle vsh = loadShader(_reader, basePath, _vsName);
        bgfx::ShaderHandle fsh = BGFX_INVALID_HANDLE;
        if (NULL != _fsName)
        {
            fsh = loadShader(_reader, basePath, _fsName);
        }

        return bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);
    }

    bgfx::ProgramHandle loadProgram(const char* basePath, const char* _vsName, const char* _fsName)
    {
        return loadProgram(entry::getFileReader(),basePath, _vsName, _fsName);
    }
}