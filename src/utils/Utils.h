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
#include <stdlib.h>
#include <memory>
#include <chrono>
#include <sstream>
#include <logic/Console.h>

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
     * @return Random number in range of -1...1
     */
    inline float frandF2()
    {
        return (rand() / (float)RAND_MAX) * 2.0f - 1.0f;
    }

    /**
     * @return Random number in range of -1...1
     */
    inline float frand()
    {
        return (rand() / (float)RAND_MAX);
    }

    /**
     * Generates a billboard
     * @param target Target data
     * @param center Center position of the quad
     * @param right Right direction and length
     * @param up Up direction and length
     */
    inline void billboardQuad(Math::float3& target0,
                       Math::float3& target1,
                       Math::float3& target2,
                       Math::float3& target3,
                       Math::float3& target4,
                       Math::float3& target5,
                       const Math::float3& center,
                       const Math::float3& right,
                       const Math::float3& up)
    {
        target0 = center + up - right; // Top/left
        target1 = center + up + right; // Top/Right
        target2 = center - up - right; // Bottom/Left

        target3 = center - up - right; // Bottom/left
        target4 = center + up + right; // Top/Right
        target5 = center - up + right; // Bottom/Right
    }

    /**
     * Strips all text starting with commentStart from the given text
     * @param json Json-text to modify
     * @param commentStart String which starts a comment
     * @return json-string without said comments
     */
    std::string stripJsonComments(const std::string& json, const std::string& commentStart = "#");

    /**
     * Checks on which side of the plane the given point is.
     * @param point Point to check
     * @param plane Plane to check against
     * @return Side the point is one. 1=front, 2=back, 3=split
     */
    inline int pointClassifyToPlane(const Math::float3& point, const Math::float4& plane)
    {
        return Math::float3(plane.v).dot(point) - plane.w < 0 ? 2 : 1;
    }

    /**
     * Checks on which side of the plane the given BBox is.
     * @param bbox  BBox to check
     * @param plane Plane to check against
     * @return Side the box is one. 1=front, 2=back, 3=split
     */
    enum EPlaneSide { PLANE_BEHIND=1, PLANE_INFRONT=2, PLANE_SPANNING=3 };
    inline EPlaneSide bboxClassifyToPlane(const BBox3D& bbox, const Math::float4& plane)
    {
        const Math::float3* const minMax[2] = { &(bbox.min), &(bbox.max) };
        int	ix = ( plane.x >= 0 ) ?  1 : 0;
        int	iy = ( plane.y >= 0 ) ?  1 : 0;
        int	iz = ( plane.z >= 0 ) ?  1 : 0;

        float decision;
        decision  = minMax[ix]->x * plane.x;
        decision += minMax[iy]->y * plane.y;
        decision += minMax[iz]->z * plane.z;

        if( decision < plane.w) {
            // BBox ist komplett links(out) der Plane => nur einen Sohn pruefen
            return PLANE_BEHIND;
        };

        decision  = minMax[1 - ix]->x * plane.x;
        decision += minMax[1 - iy]->y * plane.y;
        decision += minMax[1 - iz]->z * plane.z;
        if( decision < plane.w) {
            // BBox wird durch die Plane gesplittet => beide Soehne pruefen
            return PLANE_SPANNING;
        };
        // BBox ist komplett rechts(in) der Plane => nur einen Sohn pruefen
        return PLANE_INFRONT;
    }

    /**
     * Quick check on which sides of a plane the bbox is
     */
    inline EPlaneSide bboxClassifyToPlaneSides(const BBox3D& bbox, const Math::float4& plane)
    {
        float dist;
        Math::float3 corners[2];

        for(int i=0;i<3;i++)
        {
            if(plane.v[i] < 0.0f)
            {
                corners[0].v[i] = bbox.min.v[i];
                corners[1].v[i] = bbox.max.v[i];
            }else{
                corners[1].v[i] = bbox.min.v[i];
                corners[0].v[i] = bbox.max.v[i];
            }
        }

        dist = Math::float3(plane.v).dot(corners[0]) - plane.w; if(dist < 0.0f) { return PLANE_BEHIND; }
        dist = Math::float3(plane.v).dot(corners[1]) - plane.w; if(dist < 0.0f) { return PLANE_SPANNING; }

        return PLANE_INFRONT;
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
     * Simple class for calculating elapsed time from start() to stop()
     */
    class StopWatch
    {
    public:
        StopWatch () : m_begin(0), m_end(0), m_delayTimeStamp(0), m_stopped(true) {}
        /**
         * Start checkpoint
         */
        void start();
        /**
         * End checkpoint
         */
        void stop();
        /**
         * Returns time difference in seconds between start and now
         */
        int getTimeDiffFromStartToNow();

        bool isStopped() { return m_stopped; }
        /**
         * Returns true if delay amount of milliseconds have passed since previous invocation, false otherwise
         */
        bool DelayedByArgMS(int delay);

    private:
        long long m_begin;
        long long m_end;
        long long m_delayTimeStamp;
        bool m_stopped;
    };

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
     * Writes the given data to a file on disk
     * @param name Name of the file to write to
     * @param path Path to put the file at
     * @param data Data to put into the file
     * @return Whether the file could be written
     */
    bool writeFile(const std::string& name, const std::string& path, const std::vector<uint8_t>& data);
    bool writeFile(const std::string& name, const std::string& path, const std::string& text);

    /**
     * Reads the whole contents of a text-file into a buffer
     * @param file File to read
     * @return contents of the given textfile
     */
    std::string readFileContents(const std::string& file);

    /**
     * @return Size of the given file in bytes. 0 is not found or emtpy.
     */
    size_t getFileSize(const std::string& file);

    /**
     * Removes the last .ext from the string
     * @param fileName Input string with extension as ".ext"
     * @return String without file-extension
     */
    std::string stripExtension(const std::string& fileName);

    /**
     * Removes everything up until the last / or \\ from a string
     * @param file String to use
     * @return File without its path
     */
    std::string stripFilePath(const std::string& file);

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

    /**
     * Creates a directory on the given path
     */
    bool mkdir(const std::string& dir);

    /**
     * Grabs the directory where to put userdata like config-files or savegames.
     * Will be something like %APPDATA% on windows or $HOME on unix-based systems. Everything else will default to the current directory.
     */
    std::string getUserDataLocation();

    /**
     * Used to generate new unique Tickets.
     * Tickets may be compared or copied/assigned.
     * Tickets with different template arguments cannot be compared.
     */
    template <class T>
    class Ticket
    {
    public:
        Ticket() :
            m_ID(new char)
        {
        }

        bool operator==(const Ticket<T>& other)
        {
            return this->m_ID == other.m_ID;
        }

        bool operator!=(const Ticket<T>& other)
        {
            return !(*this == other);
        }
    protected:
        std::shared_ptr<char> m_ID;
    };

    /**
     * lower all characters inplace
     */
    void lower(std::string& in);

    /**
     *  @return lowered copy of the string
     */
    std::string lowered(const std::string& in);

    /**
     * number of equal characters at begin
     * @param a
     * @param b
     * @return number of equal characters at begin
     */
    std::size_t commonStartLength(const std::string& a, const std::string& b);

    /**
     * removes all non alphanumeric characters and lowers the case
     * @param in old string
     * @return new string
     */
    std::string strippedAndLowered(const std::string& in);

    /**
     * checks whether searchSpace contains part. ignores case, removes non alpha numeric characters first
     * @param searchSpace string to be searched in
     * @param part string to be found
     * @return
     */
    bool containsLike(const std::string& searchSpace, const std::string& part);

    /**
     * searches for the first suggestion which contains name and returns it
     * returns nullptr if not found any
     * @param suggestions suggestions to search in
     * @param name token to find
     */
    Logic::ConsoleCommand::Suggestion findSuggestion(const std::vector<Logic::ConsoleCommand::Suggestion>& suggestions,
                                                     const std::string& name);

    /**
     * performs case insensitive euqal check
     * @return true if strings are equal ignoring case
     */
    bool stringEqualIngoreCase(const std::string a, const std::string b);

    /**
     * concatenates tokens using delim inbetweeen
     * @tparam Iterator
     * @param begin
     * @param end
     * @param delim
     * @return concatenated string
     */
    template<class Iterator>
    std::string join(Iterator begin, Iterator end, const std::string& delim){
        std::stringstream ss;
        for (auto it = begin; it != end; it++)
        {
            if (it != begin)
                ss << delim;
            ss << *it;
        }
        return ss.str();
    }

    /**
     * splits s on the given delimiter and removes empty entries caused by multiple delimiters
     * @param s
     * @param delim
     * @return vector of tokens
     */
    std::vector<std::string> splitAndRemoveEmpty(const std::string &s, const char delim);

    /**
     * small class for easy to use time measurement
     */
    struct Profiler
    {
        std::string name;
        std::chrono::high_resolution_clock::time_point start;
        Profiler(const std::string& n);
        ~Profiler();
    };

    /**
     * checks if string searchSpace starts with start
     */
    bool startsWith(const std::string& searchSpace, const std::string& start);

    /**
     * checks if string searchSpace ends with end
     */
    bool endsWith(const std::string& searchSpace, const std::string& end);
}
