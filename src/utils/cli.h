#pragma once
#include <string>
#include <vector>
#include <json.hpp>

using json = nlohmann::json;

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
         * @param defaultValue default values of this flag. For booleans (nparams==0), a "0" or "1" is used.
         * @param Config section this would end up. Note that a flag only gets put into the config if this is *not*
         *        an empty string!
         */
        Flag(const std::string& flag,
             const std::string& verboseFlag = "",
             unsigned nparams = 0,
             const std::string& desc = "",
             const std::vector<std::string>& defaultValues = std::vector<std::string>(),
             std::string configSection = "");

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
         * @param Index of the argument to get
         * @return Arguments specified on the commandline of this flag. Empty string if invalid index.
         */
        const std::string& getArgs(size_t i);

        /**
         * Reads this flag from the config file. Will update m_ParsedArgs accordingly
         * @param contents Actual contents of the config file
         */
        void readFromConfig(const json& contents);

        /**
         * Writes this flag to it's section in the config-file
         * @param conf root node of the configs json data
         */
        void writeToConfig(json& conf);

        /**
         * Tries to put the desc-string into a done json-document, above this variable
         * @param configText Dumped json-text
         */
        std::string documentConfigText(const std::string& configText);
    private:

        std::string m_Flag;
        std::string m_VerboseFlag;
        std::string m_Desc;
        std::vector<std::string> m_DefaultValues;
        std::string m_ConfigSection;
        unsigned m_nParams;

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

    /**
     * Prints a list of all available flags and their usage
     */
    void printHelp();

    /**
     * Loads the config-file and updates all flags accordingly
     */
    void loadConfigFile();

    /**
     * Writes flags currently set to config file
     */
    void writeConfigFile();
}
