#include "Utils.h"
#include <tinydir.h>
#include <utils/logger.h>
#include <algorithm>
#include <cctype>
#include <bx/readerwriter.h>
#include <bx/crtimpl.h>

static bx::FileReaderI* fileReader = nullptr;
static bx::FileWriterI* fileWriter = nullptr;

void Utils::initializeFileReaderWriter()
{
    fileReader = new bx::CrtFileReader;
    fileWriter = new bx::CrtFileWriter;
}

void Utils::destroyFileReaderWriter()
{
    delete fileReader;
    fileReader = nullptr;

    delete fileWriter;
    fileWriter = nullptr;
}

void ::Utils::forEachFile(const std::string& directory, std::function<void(const std::string&,const std::string&,const std::string&)> fn, bool recursive )
{
    tinydir_dir dir;
    if (tinydir_open(&dir, directory.c_str()) == -1)
    {
        LogError() << "Failed to open directory: " << directory;
        tinydir_close(&dir);
        return;
    }

    while (dir.has_next)
    {
        tinydir_file file;
        if (tinydir_readfile(&dir, &file) == -1)
        {
            LogError() << "Error getting file";
            tinydir_close(&dir);
            return;
        }

        // Recurse, if this is a directory and we want recursion
        if (recursive
            && file.is_dir
            && std::string(file.name) != ".."
            && std::string(file.name) != ".")
            forEachFile(directory + "/" + file.name, fn, true);
        else // If not a directory, call the users function
            fn(file.path, file.name, file.extension);

        tinydir_next(&dir);
    }

    tinydir_close(&dir);
}

std::list<std::string> Utils::getFilesInDirectory(const std::string& directory, const std::string& ext, bool recursive)
{
    std::list<std::string> l;


    forEachFile(directory, [&](const std::string& path, const std::string& name, const std::string& fext){
        std::string extlower = fext;
        std::transform(extlower.begin(), extlower.end(), extlower.begin(), ::tolower);

        if(ext == "*" || extlower == ext)
            l.push_back(path);
    }, recursive);

    return l;
}

bool Utils::fileExists(const std::string& file)
{
    FILE* f = fopen(file.c_str(), "rb");

    if(!f)
        return false;

    fclose(f);

    return true;
}

std::string Utils::getCaseSensitivePath(std::string root_directory, std::string case_insensitive_path)
{
    std::string result = root_directory + case_insensitive_path;

    // first check is case insensitive path isnt right
    // if is, why should we do all of the work? :)
    if(fileExists(root_directory + case_insensitive_path)){
	return result;
    }



    std::list<std::string> file_paths = getFilesInDirectory(root_directory, "*");
    std::string lowercasepath = root_directory + case_insensitive_path;

    for(char & character : lowercasepath){
	character = std::tolower(character);
    }




    for(std::list<std::string>::iterator file_iterator=file_paths.begin(); file_iterator!=file_paths.end(); file_iterator++)
    {
	std::string lowercasepath_candidate = *file_iterator;
	for(char & character : lowercasepath_candidate){
	    character = std::tolower(character);
	}
	if(lowercasepath == lowercasepath_candidate){
	    result =  *file_iterator;
	    break;
	}
    }


    return result;

}

namespace Utils
{
static const bgfx::Memory* loadMem(bx::FileReaderI* _reader, const char* _filePath)
{
    if (bx::open(_reader, _filePath) )
    {
        uint32_t size = (uint32_t)bx::getSize(_reader);
        const bgfx::Memory* mem = bgfx::alloc(size+1);
        bx::read(_reader, mem->data, size);
        bx::close(_reader);
        mem->data[mem->size-1] = '\0';
        return mem;
    }

    //DBG("Failed to load %s.", _filePath);
    return NULL;
}

static bgfx::ShaderHandle loadShader(bx::FileReaderI* _reader, const char* _name)
{
    char filePath[512];

    const char* shaderPath = "shaders/dx9/";

    switch (bgfx::getRendererType() )
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

    strcpy(filePath, shaderPath);
    strcat(filePath, _name);
    strcat(filePath, ".bin");

    return bgfx::createShader(loadMem(_reader, filePath) );
}

bgfx::ProgramHandle loadProgram(bx::FileReaderI* _reader, const char* _vsName, const char* _fsName)
{
    bgfx::ShaderHandle vsh = loadShader(_reader, _vsName);
    bgfx::ShaderHandle fsh = BGFX_INVALID_HANDLE;
    if (NULL != _fsName)
    {
        fsh = loadShader(_reader, _fsName);
    }

    return bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);
}
} // namespace Utils

bgfx::ShaderHandle Utils::loadShader(const char* _name)
{
    return loadShader(fileReader, _name);
}

bgfx::ProgramHandle Utils::loadProgram(const char* _vsName, const char* _fsName)
{
    return loadProgram(fileReader, _vsName, _fsName);
}
