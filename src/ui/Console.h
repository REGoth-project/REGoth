//
// Created by desktop on 08.12.16.
//

#pragma once
#include <string>
#include <list>
#include <vector>
#include <functional>
#include <map>
#include <memory>
#include <ui/ConsoleBox.h>

namespace UI
{

    struct SuggestionBase
    {
        SuggestionBase(const std::vector<std::string>& aliasList) :
                aliasList(aliasList),
                anyStartsWith(false)
        {}
        virtual ~SuggestionBase() {};
        std::vector<std::string> aliasList;
        bool anyStartsWith;
    };

    struct NPCSuggestion : SuggestionBase
    {
        NPCSuggestion(const std::vector<std::string>& aliasList, Handle::EntityHandle npcHandle) :
            SuggestionBase(aliasList),
            npcHandle(npcHandle)
        {}
        Handle::EntityHandle npcHandle;
    };

    struct ConsoleCommand
    {
        // Takes list of arguments as parameter, returns command result
        using Callback = std::function<std::string(const std::vector<std::string>&)>;

        using Suggestion = std::shared_ptr<SuggestionBase>;
        // generator, which returns vector of candidates
        using CandidateListGenerator = std::function<std::vector<Suggestion>()>;

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
         * To be called when a key got pressed.
         * @param glfwKey Key ID, glfw style
         * @param mods glfw key modifier flags
         */
        void onKeyDown(int glfwKey, int mods);

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
         * @param input command to work on
         * @param limitToFixed limit the number of tokens evaluated to numFixTokens for each command
         */
        void generateSuggestions(const std::string& input, bool limitToFixed);

        /**
         * inserts the current selected suggestion
         */
        void replaceSelectedToken();

        /**
         * splits line, removes empty entries, adds empty string for lookahead trigger
         */
        static std::vector<std::string> tokenized(const std::string& line);

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
         * sets the typed line to the given value
         * @param triggerSuggestions if true, the suggestion list is recalculated
         * @param newLine new line
         */
        void setTypedLine(const std::string& newLine, bool triggerSuggestions = true);

        /**
         * Adds a message to the history
         * @param msg Message to add
         */
        void outputAdd(const std::string& msg);

        /**
         * @return Whether the console is currently shown
         */
        bool isOpen(){ return !m_ConsoleBox.isHidden(); }
        void setOpen(bool open){ m_ConsoleBox.setHidden(!open); }

        const std::list<std::string>& getOutputLines() { return m_Output; }
        const std::string& getTypedLine() { return m_TypedLine; }
        const std::vector<std::vector<UI::ConsoleCommand::Suggestion>>& getSuggestions() { return m_SuggestionsList; }
        ConsoleBox& getConsoleBox() { return m_ConsoleBox; }

    private:

        /**
         * clears the suggestion list and sets the current selection index to 0
         */
        void invalidateSuggestions();

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
        std::vector<std::vector<UI::ConsoleCommand::Suggestion>> m_SuggestionsList;

        /**
         * Currently typed line
         */
        std::string m_TypedLine;

        Engine::BaseEngine& m_BaseEngine;

        /**
         * console window
         */
        UI::ConsoleBox m_ConsoleBox;

    };
}

