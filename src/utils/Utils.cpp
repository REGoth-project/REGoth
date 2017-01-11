#include "Utils.h"
#include <fstream>
#include <tinydir.h>
#include <utils/logger.h>
#include <algorithm>
#include <cctype>
#include <bx/readerwriter.h>
#include <bx/crtimpl.h>
#include <utils/split.h>
#include <bgfx/bgfx.h>

#ifdef __unix__
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#elif _WIN32 
#include <direct.h>
#include <Shlobj.h>
#endif

const std::string USERDATA_FOLDER = ".REGoth";

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


size_t Utils::getFileSize(const std::string& file)
{
    FILE* f = fopen(file.c_str(), "rb");

    if(!f)
        return 0;

    fseek(f, 0, SEEK_END);
    size_t s = ftell(f);
    
    fclose(f);

    return s;
}

std::string Utils::getCaseSensitivePath(const std::string& caseInsensitivePath, const std::string& prePath)
{
#if defined(WIN32) || defined(_WIN32) || defined(EMSCRIPTEN)
	return prePath + caseInsensitivePath;
#else

    // Transform input path to lower
    std::string pathLower = caseInsensitivePath;
    std::transform(pathLower.begin(), pathLower.end(), pathLower.begin(), ::tolower);

    // Split the input-path at /
    std::vector<std::string> parts = Utils::split(pathLower, "/\\");

    // Get a complete directory listing
    auto getListing = [&](const std::string& directory){

        std::vector<std::string> content;

        tinydir_dir dir;
        if (tinydir_open(&dir, directory.c_str()) == -1)
        {
            tinydir_close(&dir);
            return content;
        }

        while (dir.has_next)
        {
            tinydir_file file;
            if (tinydir_readfile(&dir, &file) == -1)
            {
                tinydir_close(&dir);
                return content;
            }

            content.push_back(file.name);

            tinydir_next(&dir);
        }

        tinydir_close(&dir);
    };

    // Get the case sensitive version for every part of the path
    std::string result = caseInsensitivePath.front() == '/' ? "/." : "./.";

    if(!prePath.empty())
        result = prePath;

    for(size_t i=0;i<parts.size();i++)
    {
        std::vector<std::string> listing = getListing(result);


        bool found = false;
        for(size_t j=0;j<listing.size();j++)
        {
            // Transform to lowercase
            std::string lw = listing[j];
            std::transform(lw.begin(), lw.end(), lw.begin(), ::tolower);

            // Append the path in original casing
            if(parts[i] == lw)
            {
                found = true;
                result += "/" + listing[j];
                break;
            }
        }

        // Did not find anything?
        if(!found)
            return "";
    }

    return result;
#endif
}

namespace Utils
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

        LogWarn() << "Failed to load shader at: " <<  _filePath;
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
        return loadShader(fileReader, basePath, _name);
    }

    bgfx::ProgramHandle
    loadProgram(bx::FileReaderI* _reader, const char* basePath, const char* _vsName, const char* _fsName)
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
        return loadProgram(fileReader, basePath, _vsName, _fsName);
    }

    std::string stripExtension(const std::string& fileName)
    {
        size_t dp = fileName.find_last_of('.');
        if(dp == std::string::npos)
            return fileName;

        return fileName.substr(0, dp);
    }
}

void Utils::StopWatch::start()
{
    m_begin = m_delayTimeStamp = Utils::currentTimestamp();
    m_stopped = false;
}

void Utils::StopWatch::stop()
{
    m_end = Utils::currentTimestamp();
    m_stopped = true;
}

int Utils::StopWatch::getTimeDiffFromStartToNow()
{
    return Utils::currentTimestamp() - m_begin;
}

bool Utils::StopWatch::DelayedByArgMS(int delay)
{
    if (Utils::currentTimestamp() - m_delayTimeStamp > delay)
    {
        m_delayTimeStamp = Utils::currentTimestamp();
        return true;
    }
    else return false;
}

bool Utils::mkdir(const std::string& dir)
{
    int nError = 0;
#if defined(_WIN32)
    nError = ::_mkdir(dir.c_str()); // can be used on Windows
#else
    mode_t nMode = 0733; // UNIX style permissions
    nError = ::mkdir(dir.c_str(),nMode); // can be used on non-Windows
#endif
	return nError == 0;
}
    
std::string Utils::getUserDataLocation()
{
#ifdef _WIN32
	char buffer[MAX_PATH]; 
	BOOL result = SHGetSpecialFolderPathA(nullptr 
										, buffer
										, CSIDL_LOCAL_APPDATA
										, false );
	if(!result) return USERDATA_FOLDER; 
    return std::string(buffer) + "/" + USERDATA_FOLDER;
 
#elif __unix__
    struct passwd *pw = getpwuid(getuid());

    const char *homedir = pw->pw_dir;
    return std::string(homedir) + "/" + USERDATA_FOLDER;
#else
    return "./" + USERDATA_FOLDER;
#endif
}

std::string Utils::readFileContents(const std::string& file)
{
    std::ifstream f(file);
    std::stringstream ss;
    ss << f.rdbuf();

    return ss.str();
}
