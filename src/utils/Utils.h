#pragma once

#ifndef _MSC_VER
#include <sys/time.h>
#else

#endif
#include <math/mathlib.h>
#include <functional>
#include <list>
#include <bgfx/bgfx.h>
#include <vector>

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
     * Checks on which side of the plane the given BBox is.
     * @param bbox  BBox to check
     * @param plane Plane to check against
     * @return Side the box is one. 1=front, 2=back, 3=split
     */
    int bboxClassifyToPlane(const BBox3D& bbox, const Math::float4& plane)
    {
        float distance;
        Math::float3 corners[2];

        for(int i=0;i<3;i++)
        {
            if(plane.v[i] < 0) // Check component of planes normal vector
            {
                corners[0].v[i] = bbox.min.v[i];
                corners[1].v[i] = bbox.max.v[i];
            }else
            {
                corners[1].v[i] = bbox.min.v[i];
                corners[0].v[i] = bbox.max.v[i];
            }
        }

        
        // Test back-side
        if(Math::float3(plane.v).dot(corners[0]) - plane.w < 0)
            return 2; 

        // Test for split
        if(Math::float3(plane.v).dot(corners[1]) - plane.w < 0)
            return 3; 

        // Must be front
        return 1; 

    }

    /**
     * Modulo-operation which works for negative numbers as well
     * @return a mod b
     */
    template<typename T>
    T mod(T a, T b)
    {
        return (a%b+b)%b;
    }

    /**
     * Converts ISO-8859-1 to UTF-8
     * @param str ISO-8859-1 string
     * @return UTF-8 encoded version of the input-string
     */
    inline std::string iso_8859_1_to_utf8(const std::string &str)
    {
        std::string strOut;
        for (auto it = str.begin(); it != str.end(); ++it)
        {
            uint8_t ch = (uint8_t)*it;
            if (ch < 0x80) {
                strOut.push_back(ch);
            }
            else {
                strOut.push_back((char)(0xc0 | ch >> 6));
                strOut.push_back((char)(0x80 | (ch & 0x3f)));
            }
        }
        return strOut;
    }

    inline std::string utf8_to_iso8859_1(const char * in)
    {
        std::string out;
        if (in == NULL)
            return out;

        unsigned int codepoint;
        while (*in != 0)
        {
            unsigned char ch = static_cast<unsigned char>(*in);
            if (ch <= 0x7f)
                codepoint = ch;
            else if (ch <= 0xbf)
                codepoint = (codepoint << 6) | (ch & 0x3f);
            else if (ch <= 0xdf)
                codepoint = ch & 0x1f;
            else if (ch <= 0xef)
                codepoint = ch & 0x0f;
            else
                codepoint = ch & 0x07;
            ++in;
            if (((*in & 0xc0) != 0x80) && (codepoint <= 0x10ffff))
            {
                if (codepoint <= 255)
                {
                    out.append(1, static_cast<char>(codepoint));
                }
                else
                {
                    // do whatever you want for out-of-bounds characters
                }
            }
        }
        return out;
    }

    namespace _putArrayInternal
    {
        template<int... Is>
        struct seq { };

        template<int N, int... Is>
        struct gen_seq : gen_seq<N - 1, N - 1, Is...> { };

        template<int... Is>
        struct gen_seq<0, Is...> : seq<Is...> { };

        template<typename A, int... Is>
        static std::array<A, sizeof...(Is)> assign(A array[], seq<Is...>)
        {
            return {array[Is]...};
        }

    }

    /**
     * Assigns a given static array using an initializer-list {...} to the given target (like std::vector)
     * @param target Target to put the array into
     * @param array Array to get the data from
     */
    template<typename A, size_t SIZE>
    constexpr std::array<A, SIZE> putArray(A (&array)[SIZE])
    {
        return _putArrayInternal::assign(array, _putArrayInternal::gen_seq<SIZE>());
    }

    /**
     * Copies the values from source into array
     * @param array Standard C array (target)
     * @param source Any type indexable using []
     */
    template<typename A, size_t SIZE, typename T>
    void putArray(A (&array)[SIZE], const T& source)
    {
        for(size_t i=0; i < SIZE; i++)
        {
            array[i] = source[i];
        }
    }

    /**
     * Calculates the size of a static array
     * @param array Array to get the size from
     * @return Size of the passed static array
     */
    template<typename A, size_t SIZE>
    size_t arraySize(const A (&array)[SIZE])
    {
        return SIZE;
    }

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
     * Removes the last .ext from the string
     * @param fileName Input string with extension as ".ext"
     * @return String without file-extension
     */
    std::string stripExtension(const std::string& fileName);
    
    /**
     * Returns a string with case sensitive path based on case insensitive version
     * @param string file relative path
     * @param prePath Path to always append, which is not checked and is expected to be right
     * @return str, full sensitive path. Empty if path is invalid/could not be found
     */
    std::string getCaseSensitivePath(const std::string& case_insensitive_path, const std::string& prePath = "");

    bgfx::ShaderHandle loadShader(const char* basePath, const char* _name);

    bgfx::ProgramHandle loadProgram(const char* basePath, const char* _vsName, const char* _fsName);

    //bgfx::TextureHandle loadTexture(const char* _name, uint32_t _flags = BGFX_TEXTURE_NONE, uint8_t _skip = 0, bgfx::TextureInfo* _info = NULL);

}
