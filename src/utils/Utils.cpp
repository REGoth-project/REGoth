#include "Utils.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <tinydir.h>
#include <bgfx/bgfx.h>
#include <bx/file.h>
#include <bx/readerwriter.h>
#include <utils/logger.h>
#include <utils/split.h>

#ifdef __unix__
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#elif _WIN32
#include <Shlobj.h>
#include <direct.h>
#endif

const std::string USERDATA_FOLDER = "REGoth";

static bx::FileReaderI* fileReader = nullptr;
static bx::FileWriterI* fileWriter = nullptr;

void Utils::initializeFileReaderWriter()
{
    fileReader = new bx::FileReader;
    fileWriter = new bx::FileWriter;
}

void Utils::destroyFileReaderWriter()
{
    delete fileReader;
    fileReader = nullptr;

    delete fileWriter;
    fileWriter = nullptr;
}

void ::Utils::forEachFile(const std::string& directory, std::function<void(const std::string&, const std::string&, const std::string&)> fn, bool recursive)
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
        if (recursive && file.is_dir && std::string(file.name) != ".." && std::string(file.name) != ".")
            forEachFile(directory + "/" + file.name, fn, true);
        else  // If not a directory, call the users function
            fn(file.path, file.name, file.extension);

        tinydir_next(&dir);
    }

    tinydir_close(&dir);
}

std::list<std::string> Utils::getFilesInDirectory(const std::string& directory, const std::string& ext, bool recursive)
{
    std::list<std::string> l;

    forEachFile(directory, [&](const std::string& path, const std::string& name, const std::string& fext) {
        std::string extlower = fext;
        std::transform(extlower.begin(), extlower.end(), extlower.begin(), ::tolower);

        if (ext == "*" || extlower == ext)
            l.push_back(path);
    },
                recursive);

    return l;
}

bool Utils::fileExists(const std::string& file)
{
    FILE* f = fopen(file.c_str(), "rb");

    if (!f)
        return false;

    fclose(f);

    return true;
}

size_t Utils::getFileSize(const std::string& file)
{
    FILE* f = fopen(file.c_str(), "rb");

    if (!f)
        return 0;

    fseek(f, 0, SEEK_END);
    size_t s = ftell(f);

    fclose(f);

    return s;
}

std::string Utils::getCaseSensitivePath(const std::string& caseInsensitivePath, const std::string& prePath)
{
    // Transform input path to lower
    std::string pathLower = caseInsensitivePath;
    std::transform(pathLower.begin(), pathLower.end(), pathLower.begin(), ::tolower);

    // Split the input-path at /
    std::vector<std::string> parts = Utils::split(pathLower, "/\\");

    // Get a complete directory listing
    auto getListing = [&](const std::string& directory) {

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

            content.emplace_back(file.name);

            tinydir_next(&dir);
        }

        tinydir_close(&dir);
        return content;
    };

    // Get the case sensitive version for every part of the path
    std::string result = caseInsensitivePath.front() == '/' ? "/." : "./.";

    if (!prePath.empty())
        result = prePath;

    for (const auto &part : parts) {
        std::vector<std::string> listing = getListing(result);

        bool found = false;
        for (const auto &j : listing) {
            // Transform to lowercase
            std::string lw = j;
            std::transform(lw.begin(), lw.end(), lw.begin(), ::tolower);

            // Append the path in original casing
            if (part == lw)
            {
                found = true;
                result += "/" + j;
                break;
            }
        }

        // Did not find anything?
        if (!found)
            return "";
    }

    return result;
}

const bgfx::Memory* Utils::loadFileToMemory(const char* _filePath)
{
    if (bx::open(fileReader, _filePath))
        {
            uint32_t size = (uint32_t)bx::getSize(fileReader);
            const bgfx::Memory* mem = bgfx::alloc(size + 1);
            bx::read(fileReader, mem->data, size);
            bx::close(fileReader);
            mem->data[mem->size - 1] = '\0';
            return mem;
        }

    LogWarn() << "Failed to load file at: " << _filePath;
    return nullptr;
}

std::string Utils::stripExtension(const std::string& fileName)
{
    size_t dp = fileName.find_last_of('.');
    if (dp == std::string::npos)
        return fileName;

    return fileName.substr(0, dp);
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
    else
        return false;
}

bool Utils::mkdir(const std::string& dir)
{
    int nError = 0;
#if defined(_WIN32)
    nError = ::_mkdir(dir.c_str());  // can be used on Windows
#else
    mode_t nMode = 0733;                   // UNIX style permissions
    nError = ::mkdir(dir.c_str(), nMode);  // can be used on non-Windows
#endif
    auto errorCode = errno;
    return nError == 0 || (nError == -1 && errorCode == EEXIST);
}

std::string Utils::getUserDataLocation()
{
#ifdef _WIN32
    char buffer[MAX_PATH];
    BOOL result = SHGetSpecialFolderPathA(nullptr, buffer, CSIDL_LOCAL_APPDATA, false);
    if (!result) return USERDATA_FOLDER;
    return std::string(buffer) + "/" + USERDATA_FOLDER;

#elif ANDROID
    return "/sdcard/REGoth/userdata";
#elif __unix__
    struct passwd* pw = getpwuid(getuid());

    const char* homedir = pw->pw_dir;
    return std::string(homedir) + "/." + USERDATA_FOLDER;
#else
    return "./." + USERDATA_FOLDER;
#endif
}

std::string Utils::readFileContents(const std::string& file)
{
    std::ifstream f(file);
    std::stringstream ss;
    ss << f.rdbuf();

    return ss.str();
}

std::vector<std::uint8_t> Utils::readBinaryFileContents(const std::string& file) {
    std::ifstream inputStream(file, std::ios::binary | std::ios::ate);
    if (!inputStream.is_open()) {
        return std::vector<std::uint8_t>();
    }
    std::vector<std::uint8_t> buffer(inputStream.tellg());
    inputStream.seekg(0);
    inputStream.read((char*)buffer.data(), buffer.size());
    inputStream.close();
    return buffer;
}

bool ::Utils::writeFile(const std::string& name, const std::string& path, const std::vector<uint8_t>& data)
{
    std::string sep = (path.back() == '/' || path.back() == '\\') ? "" : "/";
    std::string target = path + sep + name;

    FILE* f = fopen(target.c_str(), "wb");
    if (!f)
        return false;

    fwrite(data.data(), data.size(), 1, f);

    fclose(f);

    return true;
}

bool ::Utils::writeFile(const std::string& name, const std::string& path, const std::string& text)
{
    std::string sep = (path.back() == '/' || path.back() == '\\') ? "" : "/";
    std::string target = path + sep + name;

    std::ofstream s(target);

    if (!s.good())
        return false;

    s << text;

    return true;
}

std::string Utils::stripFilePath(const std::string& file)
{
    if (file.find_last_of("\\/") == std::string::npos)
        return file;

    return file.substr(file.find_last_of("\\/") + 1);
}

std::string Utils::stripJsonComments(const std::string& json, const std::string& commentStart)
{
    // Split into lines
    std::vector<std::string> lines = Utils::split(json, '\n');

    std::string r;
    for (std::string& l : lines)
    {
        // Find a // and cut it off there
        l = l.substr(0, l.find(commentStart));

        r += l + "\n";
    }

    return r;
}

void Utils::lower(std::string& in)
{
    std::transform(in.begin(), in.end(), in.begin(), ::tolower);
}

std::string Utils::lowered(std::string str)
{
    Utils::lower(str);
    return str;
}

void Utils::upper(std::string& in)
{
    std::transform(in.begin(), in.end(), in.begin(), ::toupper);
}

std::string Utils::uppered(std::string str)
{
    Utils::upper(str);
    return str;
}

std::size_t Utils::commonPrefixLength(const std::string& a, const std::string& b)
{
    auto pair = std::mismatch(a.begin(), a.end(), b.begin(), b.end());
    return pair.first - a.begin();
}

std::string Utils::strippedAndLowered(std::string str)
{
    auto isNotAlNum = [](char c) { return std::isalnum(c) == 0; };
    lower(str);
    str.erase(std::remove_if(str.begin(), str.end(), isNotAlNum), str.end());
    return str;
}

bool Utils::containsLike(const std::string& searchSpace, const std::string& part)
{
    auto pos = strippedAndLowered(searchSpace).find(strippedAndLowered(part));
    return pos != std::string::npos;
}

bool Utils::stringEqualIngoreCase(const std::string a, const std::string b)
{
    if (a.size() != b.size())
        return false;
    for (std::size_t i = 0; i < a.size(); i++)
    {
        if (::tolower(a[i]) != ::tolower(b[i]))
            return false;
    }
    return true;
}

std::vector<std::string> Utils::splitAndRemoveEmpty(const std::string& s, const char delim)
{
    std::vector<std::string> tokens = Utils::split(s, delim);
    tokens.erase(std::remove(tokens.begin(), tokens.end(), ""), tokens.end());
    return tokens;
}

Utils::Profiler::Profiler(const std::string& n)
    : name(n)
    , start(std::chrono::high_resolution_clock::now())
{
}

Utils::Profiler::~Profiler()
{
    using dura = std::chrono::duration<double>;
    auto end = std::chrono::high_resolution_clock::now();
    auto d = end - start;
    LogInfo() << name << ": "
              << std::chrono::duration_cast<dura>(d).count() * 1000
              << " milliseconds";
}

bool Utils::startsWith(const std::string& searchSpace, const std::string& start)
{
    return searchSpace.find(start) == 0;
}

bool Utils::endsWith(const std::string& searchSpace, const std::string& end)
{
    auto pos = searchSpace.size() - end.size();
    return end.size() <= searchSpace.size() && searchSpace.find(end, pos) != std::string::npos;
}

void Utils::replace(std::string& searchSpace, const std::string& from, const std::string& to)
{
    std::size_t pos = 0;
    while((pos = searchSpace.find(from, pos)) != std::string::npos) {
        searchSpace.replace(pos, from.length(), to);
        pos += to.length();
    }
}

std::string Utils::replaced(std::string searchSpace, const std::string& from, const std::string& to)
{
    replace(searchSpace, from, to);
    return searchSpace;
}

std::pair<std::string, std::string> Utils::splitExtension(const std::string& filename)
{
    auto dotPos = filename.find_last_of('.');
    auto withoutExt = filename.substr(0, dotPos);
    auto ext = (dotPos != std::string::npos) ? filename.substr(dotPos, std::string::npos) : "";
    return std::make_pair(withoutExt, ext);
}

float Utils::fmod(float a, float b)
{
    return std::fmod(std::fmod(a, b) + b, b);
}

double Utils::fmod(double a, double b)
{
    return std::fmod(std::fmod(a, b) + b, b);
}

Utils::TimeSpan::TimeSpan()
    : m_elapsed(0)
    , m_depth(0)
{
}

void Utils::TimeSpan::enter()
{
    ++m_depth;
}

void Utils::TimeSpan::leave(size_t elapsed)
{
    --m_depth;
    if (m_depth == 0)
        m_elapsed += elapsed;
}

size_t Utils::TimeSpan::getAndReset()
{
    auto elapsed = m_elapsed;
    m_elapsed = 0;
    return elapsed;
}

Utils::RecursiveStopWatch::RecursiveStopWatch(Utils::TimeSpan& timeSpan, bool enabled)
    : m_TimeSpan(timeSpan)
    , m_Enabled(enabled)
    , m_Start(bx::getHPCounter())
{
    if (m_Enabled)
        m_TimeSpan.enter();
}

Utils::RecursiveStopWatch::~RecursiveStopWatch()
{
    if (m_Enabled)
    {
        auto elapsed = bx::getHPCounter() - m_Start;
        m_TimeSpan.leave(elapsed);
    }
}
