//
// Created by desktop on 12.01.17.
//

#pragma once
#include <string>

/**
 * Commandline tools for specific tasks regarding Gothic
 */
namespace zTools
{
    /**
     * Called by main. Here's the point to check your flags and run your tools.
     * @return True, if a tool was ran. The program exits in that case.
     */
    bool tryRunTools();

    /**
     * Extracts the installers used by gothic
     * @param file Installer-file on the filesystem
     * @param targetLocation Target directory for the extracted data
     * @return Whether the extraction worked
     */
    bool extractInstaller(const std::string& file, const std::string& targetLocation);
}
