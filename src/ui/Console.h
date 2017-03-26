//
// Created by desktop on 08.12.16.
//

#pragma once
#include <string>
#include <list>
#include <vector>
#include <functional>
#include <map>

namespace UI
{
    struct ConsoleCommand
    {
        // Takes list of arguments as parameter, returns command result
        typedef std::function<std::string(const std::vector<std::string>&)> Callback;
        // generator, which returns vector of candidates
        using CandidateListGenerator = std::function<std::vector<std::vector<std::string>>()>;

        std::vector<CandidateListGenerator> generators;
        Callback callback;
        unsigned int numFixTokens;
    };

    class Console
    {
    public:

        Console();

        /**
         * Updates and draws the console
         */
        void update();

        /**
         * To be called when a key got pressed.
         * @param glfwKey Key ID, glfw style
         */
        void onKeyDown(int glfwKey);

        /**
         * To be called when there was text input since the last frame
         * @param text Characters input since the last frame
         */
        void onTextInput(const std::string& text);

        /**
         * Adds a command to the console
         * @param Command Command to be added.
         * @param Callback Function to be executed if the given command was typed.
         *
         */
        void registerCommand(const std::string& command, ConsoleCommand::Callback callback);

        /**
         * Adds a command to the console
         * @param Command Command to be added.
         * @param Callback Function to be executed if the given command was typed.
         *
         */
        void registerCommand2(std::vector<ConsoleCommand::CandidateListGenerator> generators,
                              ConsoleCommand::Callback callback,
                              unsigned int numFixTokens);

        /**
         * Trigger autocompletion
         * @param line to work on
         * @param limitToFixed limit the number of tokens evaluated to numFixTokens for each command
         * @param showSuggestions show suggestions
         * @param overwriteTypedLine replace the console line with the suggested one
         * @return returns the number of the command found. -1 if non matched
         */
        int autoComplete(std::string& input, bool limitToFixed, bool showSuggestions, bool overwriteInput);

        /**
         * Executes a given command
         * @param command Command to execute
         * @return Output of the command.
         *         - "NOTFOUND", if the command was not found
         */
        std::string submitCommand(const std::string& command);

        /**
         * Adds a message to the history
         * @param msg Message to add
         */
        void outputAdd(const std::string& msg);

        /**
         * @return Whether the console is currently shown
         */
        bool isOpen(){ return m_IsOpen; }
        void setOpen(bool open){ m_IsOpen = open; }

    private:

        /**
         * Draws history over the commandline
         */
        void printOutput();

        struct
        {
            /**
             * Lines of history + current line
             */
            int height;
        }m_Config;

        /**
         * Last submitted commands
         */
        std::vector<std::string> m_History;

        /**
         * Index of the currently selected command in history
         */
        int m_HistoryIndex;

        /**
         * Line, typed before history selection
         */
        std::string m_PendingLine;

        /**
         * Lines emitted by the commands + history
         */
        std::list<std::string> m_Output;

        /**
         * All registered commands
         */
        std::vector<std::string> m_Commands;
        std::vector<ConsoleCommand> m_Commands2;

        /**
         * All registered callbacks
         */
        std::vector<ConsoleCommand::Callback> m_CommandCallbacks;

        /**
         * Currently typed line
         */
        std::string m_TypedLine;

        /**
         * Whether the console is currently shown
         */
        bool m_IsOpen;
    };
}

