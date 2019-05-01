#include "cli.h"
#include <iomanip>
#include <iostream>
#include <vector>
#include "Utils.h"
#include <cassert>
#include <cstdlib>
#include <ZenLib/utils/logger.h>
#include <ZenLib/utils/split.h>

using namespace Cli;

const std::string CONFIG_FILE = "config.json";
const int JSON_DUMP_INDENTATION = 4;  // How many spaces to make the json pretty

namespace Global
{
    /**
     * @return List of all registered flags
     */
    std::vector<Flag*>& getFlagList()
    {
        static std::vector<Flag*> flagList;  // Static so it won't be initialized after pushing anything to it
        return flagList;
    }

    /**
     * Current commandline, split on spaces
     */
    std::vector<std::string> commandline;
}

Flag::Flag(const std::string& flag,
           const std::string& verboseFlag,
           unsigned nparams,
           const std::string& desc,
           const std::vector<std::string>& defaultValues,
           std::string configSection)
    : m_Flag(flag)
    , m_VerboseFlag(verboseFlag)
    , m_nParams(nparams)
    , m_Desc(desc)
    , m_DefaultValues(defaultValues)
    , m_ParsedArgs(defaultValues)
    ,  // Initialize with default values
    m_ConfigSection(configSection)
{
    if (!flag.empty())
        assert(flag.find_first_not_of(' ') != std::string::npos);

    if (!verboseFlag.empty())
        assert(verboseFlag.find_first_not_of(' ') != std::string::npos);

    // Add this flag to the global list
    Global::getFlagList().push_back(this);
}

std::vector<std::string> Flag::extractFlag()
{
    for (size_t i = 0; i < Global::commandline.size(); i++)
    {
        if (Global::commandline[i] != "-" + m_Flag && Global::commandline[i] != "--" + m_VerboseFlag)
            continue;
        m_SetLocation = (int)i;

        // Found it! Now check if the argumentcount is right
        unsigned k = 0;
        std::vector<std::string> args;
        for (size_t j = i + 1; j < Global::commandline.size(); j++, k++)
        {
            if (Global::commandline[j].front() == '-')
                break;

            args.push_back(Global::commandline[j]);
        }

        if (k != m_nParams)
        {
            std::cout << "ARGPARSE: '" << m_Flag << "'" << (m_VerboseFlag.empty() ? "" : ", ('" + m_VerboseFlag + "')") << " - wrong argument count. Expected "
                      << m_nParams << ", found " << k << std::endl;

            printUsage();
            exit(0);
        }
        else
        {
            m_ParsedArgs = args;
        }
    }

    return {};
}

bool Flag::isSet()
{
    if (m_nParams == 0 && m_SetLocation == 0 && !m_ParsedArgs.empty())
    {
        // There could be args read from config for this boolean.
        // Booleans use "hidden" parsed-args to store their value in the config file
        if (m_ParsedArgs[0] == "1")
            return true;
        else if (m_ParsedArgs[0] == "0")
            return false;
    }

    return m_SetLocation != 0 || (m_nParams != 0 && m_ParsedArgs.size() == m_nParams);
}

void Flag::printUsage()
{
    std::string flag = (m_Flag.empty() ? "" : "-" + m_Flag + "");
    std::string verboseFlag = (m_VerboseFlag.empty() ? "" : "--" + m_VerboseFlag + "");
    std::string flagInfo;

    if (!flag.empty())
        flagInfo += flag;

    if (!flagInfo.empty() && !verboseFlag.empty())
        flagInfo += ", ";

    if (!verboseFlag.empty())
        flagInfo += verboseFlag;

    std::cout << std::left << std::setw(25) << flagInfo;

    std::cout << std::setw(1) << ": ";
    std::cout << std::left << (m_nParams > 1 ? ("[" + std::to_string(m_nParams) + " args] ") : "") << m_Desc << std::endl;
}

void Flag::readFromConfig(const json& contents)
{
    // Choose the verbose version over the smaller one
    std::string flag = m_VerboseFlag.empty() ? m_Flag : m_VerboseFlag;

    if (contents.find(m_ConfigSection) == contents.end() || contents[m_ConfigSection].find(flag) == contents[m_ConfigSection].end())
    {
        // It's not there, use the default
        m_ParsedArgs = m_DefaultValues;
        return;
    }

    const json& jf = contents[m_ConfigSection][flag];

    LogInfo() << "Reading Flag '" << flag << " = " << jf.dump() << "' from config";

    // Flags can be either arrays or single entries
    if (!jf.is_array())
    {
        m_ParsedArgs.clear();
        if (jf.is_boolean())
        {
            m_ParsedArgs.emplace_back(contents[m_ConfigSection][flag] ? "1" : "0");  // Doesn't matter where exactly this ends up in the commandline, this has to be non-0 to count as "set"
        }
        else
        {
            m_ParsedArgs.push_back(contents[m_ConfigSection][flag]);
        }
    }
    else
    {
        m_ParsedArgs = contents[m_ConfigSection][flag].get<std::vector<std::string>>();
    }
}

void Flag::writeToConfig(json& conf)
{
    // Choose the verbose version over the smaller one
    std::string flag = m_VerboseFlag.empty() ? m_Flag : m_VerboseFlag;

    if (m_DefaultValues.empty() || m_ConfigSection.empty())
        return;  // Don't write these into the config, they don't want to be there

    if (conf.find(m_ConfigSection) == conf.end())
        conf[m_ConfigSection] = json::object();

    if (m_nParams == 0)
        conf[m_ConfigSection][flag] = isSet();
    else if (m_nParams == 1)
        conf[m_ConfigSection][flag] = m_ParsedArgs[0];
    else
        conf[m_ConfigSection][flag] = m_ParsedArgs;
}

std::string Flag::documentConfigText(const std::string& configText)
{
    std::vector<std::string> lines = Utils::split(configText, '\n');
    std::vector<std::string> docLines = Utils::split(m_Desc, '\n');
    std::vector<std::string> actualLines;
    std::string flag = m_VerboseFlag.empty() ? m_Flag : m_VerboseFlag;

    // Find a line with our flag
    for (auto &line : lines) {
        if (line.find("\"" + flag + "\": ") != std::string::npos)
        {
            // This is our line! Add our desc above it
            for (const std::string& d : docLines)
            {
                // Indent and prefix with #
                std::string c = "\n";                         // Add one free line between the comment and the previous line
                c.insert(1, JSON_DUMP_INDENTATION * 2, ' ');  // 2 indents because of section
                c += "# " + d;
                actualLines.push_back(c);
            }
        }

        actualLines.push_back(line);
    }

    // Merge lines to text again
    std::string r;
    for (const std::string& l : actualLines)
        r += l + "\n";

    return r;
}

const std::string& Flag::getParam(unsigned i)
{
    static const std::string emptyString = "";
    if (i >= m_ParsedArgs.size())
        return emptyString;

    return m_ParsedArgs[i];
}

void ::Cli::setCommandlineArgs(int argc, char** argv)
{
    for (int i = 0; i < argc; i++)
        Global::commandline.emplace_back(argv[i]);

    // Check all flags for correctness
    for (Flag* f : Global::getFlagList())
        f->extractFlag();
}

void ::Cli::printHelp()
{
    std::cout << "Usage:" << std::endl;

    for (Flag* f : Global::getFlagList())
        f->printUsage();
}

void ::Cli::loadConfigFile()
{
    std::string conf = Utils::getUserDataLocation() + "/" + CONFIG_FILE;

    // Have we even written one yet?
    if (Utils::fileExists(conf))
    {
        std::string contents = Utils::readFileContents(conf);

        // Strip all comments since the json-parser doesn't support them
        std::string cleaned = Utils::stripJsonComments(contents);

        if (cleaned.empty())
            return;  // Guess that's not valid

        json j = json::parse(cleaned);

        for (Flag* f : Global::getFlagList())
            f->readFromConfig(j);
    }
}

void ::Cli::writeConfigFile()
{
    json j = json::object();

    // Write each flag
    for (Flag* f : Global::getFlagList())
        f->writeToConfig(j);

    // Dump using specified indentation. This is important for comments, since they use the same indentation!
    std::string dumped = j.dump(JSON_DUMP_INDENTATION);

    // Another pass, to insert the comments
    for (Flag* f : Global::getFlagList())
        dumped = f->documentConfigText(dumped);

    // Write that one to disk now. Ensure the userdata-folder exists first, though
    std::string userdata = Utils::getUserDataLocation();
    if (!Utils::mkdir(userdata))
        LogError() << "Failed to create userdata-directory at: " << userdata;

    // Now create the file
    if (!Utils::writeFile("config.json", userdata, dumped))
        LogError() << "Failed to write config-file to: " << userdata << "/config.json";
}
