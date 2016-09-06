#include "Utils.h"
#include <tinydir.h>
#include <utils/logger.h>
#include <algorithm>
#include <cctype>
#include <utils/split.h>


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

std::string Utils::getCaseSensitivePath(const std::string& caseInsensitivePath)
{
#if defined(WIN32) || defined(_WIN32)
	return caseInsensitivePath;
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
