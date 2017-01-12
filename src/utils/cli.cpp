#include <vector>
#include <assert.h>
#include <ZenLib/utils/split.h>
#include <iostream>
#include <stdlib.h>
#include "cli.h"

using namespace Cli;

namespace Global
{
    /**
     * @return List of all registered flags
     */
    std::vector<Flag*>& getFlagList()
    {
        static std::vector<Flag*> flagList; // Static so it won't be initialized after pushing anything to it
        return flagList;
    }

    /**
     * Current commandline, split on spaces
     */
    std::vector<std::string> commandline;
}

Flag::Flag(const std::string& flag, const std::string& verboseFlag, int nparams, const std::string& desc)
        : m_Flag(flag), m_VerboseFlag(verboseFlag), m_nParams(nparams), m_Desc(desc)
{
    assert(!flag.empty());
    assert(!flag.find_first_not_of(' ') != std::string::npos);

    std::cout << "Registered flag: -" << flag;
    if(!verboseFlag.empty())
    {
        assert(!flag.find_first_not_of(' ') != std::string::npos);

        std::cout << " (verbose: --" << verboseFlag << ")";
    }
    std::cout << std::endl;

    // Add this flag to the global list
    Global::getFlagList().push_back(this);
}

std::vector<std::string> Flag::extractFlag()
{
    for(size_t i = 0; i < Global::commandline.size(); i++)
    {
        if(Global::commandline[i] == "-" + m_Flag
           || Global::commandline[i] == "--" + m_VerboseFlag)
        {
            m_SetLocation = (int)i;

            // Found it! Now check if the argumentcount is right
            int k = 0;
            std::vector<std::string> args;
            for(size_t j = i + 1; j<Global::commandline.size(); j++, k++)
            {
                if(Global::commandline[j].front() == '-')
                    break;

                args.push_back(Global::commandline[j]);
            }

            if(k != m_nParams)
            {
                std::cout << "ARGPARSE: '" << m_Flag << "'" << (m_VerboseFlag.empty() ? "" : ", ('" + m_VerboseFlag + "')") << " - wrong argument count. Expected "
                          << m_nParams << ", found " << k << std::endl;

                printUsage();
                exit(0);
            }else
            {
                m_ParsedArgs = args;
            }
        }
    }

    return {};
}

bool Flag::isSet()
{
    return m_SetLocation != 0;
}

void Flag::printUsage()
{
    std::cout << "-" + m_Flag << (m_VerboseFlag.empty() ? "" : ", --" + m_VerboseFlag + "") << "\t: " << m_Desc << std::endl;
}

void ::Cli::setCommandlineArgs(int argc, char** argv)
{
    for(int i = 0; i < argc; i++)
        Global::commandline.push_back(std::string(argv[i]));

    // Check all flags for correctness
    for(Flag* f : Global::getFlagList())
        f->extractFlag();
}
