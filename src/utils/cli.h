#pragma once
#include <string>
#include <vector>

/**
 * Simple interface to commandline flags and options
 */
namespace Cli
{
    /**
     * Wrapper class for a commandline-flags, eg. "./app -f" or "./app --do-foo"
     */
    class Flag
    {
    public:
        /**
         * Adds the given flag to the internal list
         * @param flag Flag used on the commandline without any dashes., with eg. "./app -f",
         *             this would have to be "f". Can be longer than one character, but should be kept short.
         * @param verboseFlag Verbose version of the short-flag given beforehand, with eg. "./app --do-foo",
         *             this would have to be "do-foo".
         * @param nparams number of parameters of the flag
         */
        Flag(const std::string& flag, const std::string& verboseFlag = "", int nparams = 0, const std::string& desc = "");

        /**
         * @return Whether this flag was given on the commandline
         */
        bool isSet();

        /**
         * Prints the usage information for this flag
         */
        void printUsage();

        /**
         * Tries to extract the flag from the given commandline
         * @return Flag + Arguments. Empty if not found, exits if invalid.
         */
        std::vector<std::string> extractFlag();

        /**
         * @return Arguments specified on the commandline of this flag
         */
        std::vector<std::string> getArgs(){ return m_ParsedArgs; }
    private:

        std::string m_Flag;
        std::string m_VerboseFlag;
        std::string m_Desc;
        int m_nParams;

        /**
         * Location this flag is in the argv-array. If this is not 0, the flag is set!
         */
        int m_SetLocation;

        std::vector<std::string> m_ParsedArgs;
    };

    /**
     * Call on start to let the commands registered here know about the given commandline
     * @param args Commandline arguments seperated by spaces
     */
    void setCommandlineArgs(int argc, char** argv);
}
