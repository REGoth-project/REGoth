#pragma once

#ifndef _MSC_VER
#include <sys/time.h>
#else

#endif
#include <math/mathlib.h>
#include <functional>
#include <list>
#include <bgfx/bgfx.h>

namespace Utils
{

    void initializeFileReaderWriter();
    void destroyFileReaderWriter();

    struct BBox3D
    {
        Math::float3 min;
        Math::float3 max;
    };

    /**
     * Rounds the given float to the nearest integer of type T
     * @param f Float to round
     * @return f rounded to the nearest integer
     */
    template<typename T>
    T round(float f)
    {
        return static_cast<T>(f + 0.5f);
    }

    /**
     * @return The current timestamp since some time in milliseconds
     */
    static long long currentTimestamp() {
#ifndef _MSC_VER
        struct timeval te;
        gettimeofday(&te, nullptr); // get current time
        long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // caculate milliseconds

        return milliseconds;
#else
        // TODO: Implement? BGFX has this...
		return 0;
#endif
    }

    /**
     * Iterates through the given directory and calls fn for each file
     * @param directory path to directory to interate through
     * @param fn function to call for each file. Arguments: full path, filename, ext
     * @param recursive recursive search
     */
    void forEachFile(const std::string& directory, std::function<void(const std::string&,const std::string&,const std::string&)> fn, bool recursive = true);

    /**
     * Returns a list with paths to all files found inside the given directory
     * @param directory The directory to search through
     * @param ext File-Extension to filter. Use * to include every extension
     * @param recursive Whether to continue the search in other directories found inside the one given
     * @return List of paths to all files found inside our directory
     */
    std::list<std::string> getFilesInDirectory(const std::string& directory, const std::string& ext = "*", bool recursive = true);

    /**
     * Checks whether the given file exists
     * @param file File to check
     * @return True, if the given file can be opened
     */
    bool fileExists(const std::string& file);
    
    
    /**
     * Returns a string with case sensitive path based on case insensitive version
     * @param string game root directory
     * @param string file relative path
     * @return str, full sensitive path 
     */
    std::string getCaseSensitivePath(std::string root_directory, std::string case_insensitive_path);

    bgfx::ShaderHandle loadShader(const char* _name);

    bgfx::ProgramHandle loadProgram(const char* _vsName, const char* _fsName);

    //bgfx::TextureHandle loadTexture(const char* _name, uint32_t _flags = BGFX_TEXTURE_NONE, uint8_t _skip = 0, bgfx::TextureInfo* _info = NULL);

}
