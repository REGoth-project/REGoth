#include <bgfx/bgfx.h>
#include <ZenLib/utils/split.h>
#include <utils/logger.h>
#include "Console.h"
#include <iterator>
#include <utils/Utils.h>
#include <iomanip>
#include <engine/BaseEngine.h>
#include <GLFW/glfw3.h>
#include <ui/Hud.h>

using namespace UI;

/* Function keys */
namespace Keys
{
    // All keys mapped to ascii-characters
    const int PrintableBegin = 32;
    const int PrintableEnd = 93; // Inclusive
};

Console::Console(Engine::BaseEngine& e) :
    m_BaseEngine(e)
{
    m_HistoryIndex = 0;
    m_Open = false;
    outputAdd(" ----------- REGoth Console -----------");
}

Console::~Console() {
}

void Console::onKeyDown(int glfwKey, int mods)
{
    auto& consoleBox = m_BaseEngine.getHud().getConsoleBox();
    if(glfwKey == GLFW_KEY_PAGE_DOWN)
    {
        consoleBox.increaseSelectionIndex(1);
    }
    if(glfwKey == GLFW_KEY_PAGE_UP)
    {
        consoleBox.increaseSelectionIndex(-1);
    }
    if(glfwKey == GLFW_KEY_HOME)
    {
        consoleBox.setSelectionIndex(0);
    }
    if(glfwKey == GLFW_KEY_END)
    {
        if (!m_SuggestionsList.empty())
        {
            consoleBox.setSelectionIndex(m_SuggestionsList.back().size() - 1);
        }
    }
    if(glfwKey == GLFW_KEY_UP)
    {
        const int historySize = m_History.size();
        if(historySize > m_HistoryIndex + 1)
        {
            if (m_HistoryIndex < 0)
                m_PendingLine = m_TypedLine;
            ++m_HistoryIndex;
            auto newLine = m_History.at(m_History.size() - m_HistoryIndex - 1);
            setTypedLine(newLine, false);
        }
    }else if(glfwKey == GLFW_KEY_DOWN)
    {
        if(m_HistoryIndex >= 0)
        {
            --m_HistoryIndex;
            std::string newLine;
            if (m_HistoryIndex < 0)
                newLine = m_PendingLine;
            else
                newLine = m_History.at(m_History.size() - m_HistoryIndex - 1);
            setTypedLine(newLine, false);
        }
    }else if(glfwKey == GLFW_KEY_BACKSPACE)
    {
        if(m_TypedLine.size() >= 1)
        {
            m_TypedLine.pop_back();
            generateSuggestions(m_TypedLine, false);
        }
    }else if(glfwKey == GLFW_KEY_ENTER)
    {
        if (consoleBox.getSelectionIndex() != -1)
        {
            replaceSelectedToken();
            consoleBox.setSelectionIndex(-1);
        } else
        {
            submitCommand(m_TypedLine);
            setTypedLine("", false);
        }
    }else if(glfwKey == GLFW_KEY_TAB)
    {
        if (m_SuggestionsList.empty())
        {
            generateSuggestions(m_TypedLine, false);
        }
        else if (!m_SuggestionsList.back().empty())
        {
            // default for now: only auto complete last token
            const auto& suggestions = m_SuggestionsList.back();
            if (suggestions.size() == 1)
            {
                consoleBox.setSelectionIndex(0);
                replaceSelectedToken();
            } else {
                if (consoleBox.getSelectionIndex() == -1)
                {
                    // nothing is selected -> select first element
                    consoleBox.setSelectionIndex(0);
                    if (mods & GLFW_MOD_SHIFT)
                        consoleBox.increaseSelectionIndex(-1);
                } else {
                    consoleBox.increaseSelectionIndex(mods & GLFW_MOD_SHIFT ? -1 : 1);
                }
            }
        }
    }
}

void Console::onTextInput(const std::string& text)
{
    if (!text.empty())
        setTypedLine(m_TypedLine + text);
}

std::string Console::submitCommand(std::string command)
{
    std::vector<std::string> args = Utils::splitAndRemoveEmpty(command, ' ');
    if(!args.empty() && (m_History.empty() || m_History.back() != command))
        m_History.push_back(command);

    m_HistoryIndex = -1;
    m_PendingLine.clear();

    if(args.empty())
        return "";

    // bool autoCompleteCommandTokensOnly = true;
    // autoComplete(command, autoCompleteCommandTokensOnly, true);

    outputAdd(">> " + command);

    auto commID = determineCommand(args);

    if (commID != -1)
    {
        std::string result;
        try {
            result = m_Commands.at(commID).callback(args);
        } catch (const std::out_of_range& e)
        {
            result = "error: argument out of range";
        } catch (const std::invalid_argument& e)
        {
            result = "error: invalid argument";
        }
        outputAdd(result);
        return result;
    }

    outputAdd("-- Command not found --");
    return "NOTFOUND";
}

ConsoleCommand& Console::registerCommand(const std::string& command, ConsoleCommand::Callback callback)
{
    auto tokens = Utils::splitAndRemoveEmpty(command, ' ');
    std::vector<ConsoleCommand::CandidateListGenerator> generators;
    auto simpleGen = [](std::string token) -> std::vector<ConsoleCommand::Suggestion> {
        ConsoleCommand::Suggestion suggestion = std::make_shared<SuggestionBase>(SuggestionBase {{token}});
        return {suggestion};
    };
    for (auto tokenIt = tokens.begin(); tokenIt != tokens.end(); tokenIt++)
    {
        generators.push_back(std::bind(simpleGen, *tokenIt));
    }
    auto sanitizedCommand = Utils::join(tokens.begin(), tokens.end(), " ");
    m_Commands.emplace_back(ConsoleCommand{sanitizedCommand, generators, callback, generators.size()});
    return m_Commands.back();
}

void Console::outputAdd(const std::string& msg)
{
    m_Output.push_front(msg);
}

int Console::determineCommand(const std::vector<std::string>& tokens)
{
    std::vector<std::size_t> numMatchingTokens(m_Commands.size(), 0);
    for (std::size_t commID = 0; commID < m_Commands.size(); commID++)
    {
        auto& command = m_Commands.at(commID);
        if (command.numFixTokens > tokens.size())
            continue;
        bool allStagesMatched = true;
        for (std::size_t tokenID = 0; tokenID < command.numFixTokens; tokenID++)
        {
            auto suggestion = Utils::findSuggestion(command.generators.at(tokenID)(), tokens.at(tokenID));
            if (suggestion == nullptr)
                allStagesMatched = false;
        }
        if (allStagesMatched)
        {
            numMatchingTokens[commID] = command.numFixTokens;
        }
    }
    auto itMaxelement = std::max_element(numMatchingTokens.begin(), numMatchingTokens.end());
    if (*itMaxelement != 0)
    {
        // case: at least one command matched with command.numFixTokens
        auto commID = itMaxelement - numMatchingTokens.begin();
        return static_cast<int>(commID);
    }
    return -1;
}

/*
int naturalComparator(const std::string& left, const std::string& right)
{
    auto leftVec = Utils::splitAndRemoveEmpty(left, '_');
    auto rightVe = Utils::splitAndRemoveEmpty(right, '_');
}*/

using Suggestion = UI::ConsoleCommand::Suggestion;

void Console::generateSuggestions(const std::string& input, bool limitToFixed) {
    using std::vector;
    using std::string;

    vector<string> tokens = tokenized(input);

    vector<vector<Suggestion>> suggestionsList;

    auto suggestionCompare = [](const Suggestion& a, const Suggestion& b) -> int {
        int anyStartsWithRelation = b->anyStartsWith - a->anyStartsWith;
        if (anyStartsWithRelation != 0)
            return anyStartsWithRelation;
        auto minSize = std::min(a->aliasList.size(), b->aliasList.size());
        for (std::size_t i = 0; i < minSize; ++i)
        {
            auto relation = a->aliasList[0].compare(b->aliasList[0]);
            if (relation != 0)
                return relation;
        }
        return static_cast<int>(a->aliasList.size()) - static_cast<int>(b->aliasList.size());
    };

    vector<bool> commandIsAlive(m_Commands.size(), true);
    for (std::size_t tokenID = 0; tokenID < tokens.size(); tokenID++)
    {
        const string& token = tokens[tokenID];
        auto tokenLowered = Utils::lowered(token);
        vector<Suggestion> matches;
        for (std::size_t cmdID = 0; cmdID < m_Commands.size(); cmdID++)
        {
            auto& command = m_Commands[cmdID];
            auto& generators = command.generators;
            std::size_t cmdEnd = limitToFixed ? command.numFixTokens : generators.size();
            if (commandIsAlive[cmdID] && tokenID < cmdEnd)
            {
                commandIsAlive[cmdID] = false;
                auto suggestions = generators[tokenID]();
                for (auto& suggestion : suggestions)
                {
                    bool matched = false;
                    for (auto& candidate : suggestion->aliasList)
                    {
                        auto pos = Utils::lowered(candidate).find(tokenLowered);
                        if (!candidate.empty() && pos != std::string::npos)
                        {
                            commandIsAlive[cmdID] = true;
                            matched = true;
                            if (pos == 0)
                                suggestion->anyStartsWith = true;
                        }
                    }
                    if (matched)
                        matches.push_back(suggestion);
                }
            }
        }
        // generate suggestions
        {
            std::sort(matches.begin(), matches.end(), [suggestionCompare](const Suggestion& a, const Suggestion& b) -> bool{
                return suggestionCompare(a, b) < 0;
            });
            // filter out duplicates in consecutive groups
            auto uniquePredicate = [suggestionCompare](const Suggestion& a, const Suggestion& b) -> bool{
                return a->aliasList == b->aliasList;
            };
            matches.erase(std::unique(matches.begin(), matches.end(), uniquePredicate), matches.end());
            suggestionsList.push_back(std::move(matches));
        }
    }
    invalidateSuggestions();
    m_SuggestionsList = suggestionsList;
}

void Console::invalidateSuggestions() {
    m_BaseEngine.getHud().getConsoleBox().setSelectionIndex(-1);
    m_SuggestionsList.clear();
}

std::vector<std::string> Console::tokenized(const std::string &line) {
    std::vector<std::string> tokens = Utils::splitAndRemoveEmpty(line, ' ');
    if (tokens.empty() || isspace(line.back()))
    {
        // append empty pseudo token to trigger lookahead for the next token
        tokens.push_back("");
    }
    return tokens;
}

void Console::replaceSelectedToken() {
    // default for now: only auto complete last token
    const auto& suggestions = m_SuggestionsList.back();
    auto selectedSuggestion = suggestions.at(m_BaseEngine.getHud().getConsoleBox().getSelectionIndex());
    std::vector<std::string> tokens = tokenized(m_TypedLine);
    tokens.back() = selectedSuggestion->aliasList.at(0);
    std::string newLine = Utils::join(tokens.begin(), tokens.end(), " ");
    if (!tokens.empty())
    {
        newLine += " ";
    }
    setTypedLine(newLine);
}

void Console::setTypedLine(const std::string &newLine, bool triggerSuggestions) {
    m_TypedLine = newLine;
    invalidateSuggestions();
    if (triggerSuggestions)
        generateSuggestions(m_TypedLine, false);
}

void Console::setOpen(bool open) {
    m_Open = open;
    if (!open)
        invalidateSuggestions();
}


