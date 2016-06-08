#include "Utils.h"
#include <tinydir.h>
#include <utils/logger.h>

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
        if(ext == "*" || fext == ext)
            l.push_back(path);
    });

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
