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

namespace Logic
{
    class Console
    {
    public:

        struct SuggestionBase
        {
            SuggestionBase(const std::vector<std::string>& aliasList) :
                    aliasList(aliasList),
                    anyStartsWith(false)
            {}

            virtual ~SuggestionBase() {};

            bool operator<(const SuggestionBase& b) const;

            std::vector<std::string> aliasList;
            bool anyStartsWith;
        };

        using Suggestion = std::shared_ptr<SuggestionBase>;

        // Takes list of arguments as parameter, returns command result
        using Callback = std::function<std::string(const std::vector<std::string>&)>;

        // generator, which returns vector of candidates
        using CandidateListGenerator = std::function<std::vector<Suggestion>()>;

        struct NPCSuggestion : SuggestionBase
        {
            NPCSuggestion(const std::vector<std::string>& aliasList, Handle::EntityHandle npcHandle) :
                    SuggestionBase(aliasList),
                    npcHandle(npcHandle)
            {}
            Handle::EntityHandle npcHandle;
        };

        struct Command
        {
            std::string commandName;
            std::vector<CandidateListGenerator> generators;
            Callback callback;

            // number of tokens, that must match to identify this command
            std::size_t numFixTokens;

            // whether this command is disabled while no world is loaded
            bool requiresWorld;

            Command& setRequiresWorld(bool value)
            {
                requiresWorld = value;
                return *this;
            }

            Command& registerAutoComplete(CandidateListGenerator generator)
            {
                generators.push_back(generator);
                return *this;
            }
        };


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
         * @return reference to the created command, which will never be invalidated (commands are stored in std::list)
         */
        Command& registerCommand(const std::string& command, Callback callback);

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
        std::list<Command>::iterator determineCommand(const std::vector<std::string>& tokens);

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
        bool isOpen(){ return m_Open; }
        void setOpen(bool open);
        void toggleOpen(){ setOpen(!isOpen()); }

        const std::list<std::string>& getOutputLines() { return m_Output; }
        const std::string& getTypedLine() { return m_TypedLine; }
        const std::vector<std::vector<Logic::Console::Suggestion>>& getSuggestions() { return m_SuggestionsList; }

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
        std::list<Command> m_Commands;

        /**
         * suggestions for each token
         */
        std::vector<std::vector<Logic::Console::Suggestion>> m_SuggestionsList;

        /**
         * Currently typed line
         */
        std::string m_TypedLine;

        /**
         * Console active
         */
        bool m_Open;

        Engine::BaseEngine& m_BaseEngine;
    };
}

