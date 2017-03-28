//
// Created by desktop on 08.12.16.
//

#pragma once
#include <string>
#include <list>
#include <vector>
#include <functional>
#include <map>
#include <ui/ConsoleBox.h>

namespace UI
{
    struct ConsoleCommand
    {
        // Takes list of arguments as parameter, returns command result
        typedef std::function<std::string(const std::vector<std::string>&)> Callback;
        // generator, which returns vector of candidates
        using CandidateListGenerator = std::function<std::vector<std::vector<std::string>>()>;

        std::string commandName;
        std::vector<CandidateListGenerator> generators;
        Callback callback;
        // number of tokens, that must match to identify this command
        std::size_t numFixTokens;

        ConsoleCommand& registerAutoComplete(CandidateListGenerator generator){
            generators.push_back(generator);
            return *this;
        }
    };

    class Console
    {
    public:

        Console(Engine::BaseEngine& e);

        ~Console();

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
        ConsoleCommand& registerCommand(const std::string& command, ConsoleCommand::Callback callback);

        /**
         * Trigger autocompletion
         * @param command to work on
         * @param limitToFixed limit the number of tokens evaluated to numFixTokens for each command
         * @param showSuggestions show suggestions
         * @param overwriteTypedLine replace the console line with the suggested one
         */
        using Suggestion = std::vector<std::string>;
        std::vector<std::vector<Suggestion>> autoComplete(std::string& input, bool limitToFixed, bool overwriteInput);

        /**
         * searches for command which, could generate the given tokens and returns its index
         */
        int determineCommand(const std::vector<std::string>& tokens);

        /**
         * Executes a given command
         * @param command Command to execute
         * @return Output of the command.
         *         - "NOTFOUND", if the command was not found
         */
        std::string submitCommand(std::string command);

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

        const std::list<std::string>& getOutputLines() { return m_Output; }
        const std::string& getTypedLine() { return m_TypedLine; }
        const std::vector<std::vector<Suggestion>>& getSuggestions() { return m_SuggestionsList; }

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
        std::vector<ConsoleCommand> m_Commands;

        /**
         * suggestions for each token
         */
        std::vector<std::vector<Suggestion>> m_SuggestionsList;

        /**
         * Currently typed line
         */
        std::string m_TypedLine;

        /**
         * Whether the console is currently shown
         */
        bool m_IsOpen;

        Engine::BaseEngine& m_BaseEngine;

        /**
         * console window
         */
        UI::ConsoleBox m_ConsoleBox;

    };
}

