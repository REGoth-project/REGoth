#include <ZenLib/utils/split.h>
#include <utils/logger.h>
#include "Console.h"
#include <utils/Utils.h>
#include <engine/BaseEngine.h>
#include <GLFW/glfw3.h>
#include <ui/Hud.h>

using Logic::Console;

Console::Console(Engine::BaseEngine& e)
    : m_BaseEngine(e)
{
    m_HistoryIndex = 0;
    m_Open = false;
    outputAdd(" ----------- REGoth Console -----------");
}

Console::~Console()
{
}

void Console::onKeyDown(int glfwKey, int mods)
{
    auto& consoleBox = m_BaseEngine.getHud().getConsoleBox();
    if (glfwKey == GLFW_KEY_PAGE_DOWN)
    {
        consoleBox.increaseSelectionIndex(1);
    }
    if (glfwKey == GLFW_KEY_PAGE_UP)
    {
        consoleBox.increaseSelectionIndex(-1);
    }
    if (glfwKey == GLFW_KEY_HOME)
    {
        consoleBox.setSelectionIndex(0);
    }
    if (glfwKey == GLFW_KEY_END)
    {
        if (!m_SuggestionsList.empty())
        {
            consoleBox.setSelectionIndex(m_SuggestionsList.back().size() - 1);
        }
    }
    if (glfwKey == GLFW_KEY_UP)
    {
        const int historySize = m_History.size();
        if (historySize > m_HistoryIndex + 1)
        {
            if (m_HistoryIndex < 0)
                m_PendingLine = m_TypedLine;
            ++m_HistoryIndex;
            auto newLine = m_History.at(m_History.size() - m_HistoryIndex - 1);
            setTypedLine(newLine, false);
        }
    }
    else if (glfwKey == GLFW_KEY_DOWN)
    {
        if (m_HistoryIndex >= 0)
        {
            --m_HistoryIndex;
            std::string newLine;
            if (m_HistoryIndex < 0)
                newLine = m_PendingLine;
            else
                newLine = m_History.at(m_History.size() - m_HistoryIndex - 1);
            setTypedLine(newLine, false);
        }
    }
    else if (glfwKey == GLFW_KEY_BACKSPACE)
    {
        if (m_TypedLine.size() >= 1)
        {
            m_TypedLine.pop_back();
            generateSuggestions(m_TypedLine, false);
        }
    }
    else if (glfwKey == GLFW_KEY_ENTER)
    {
        if (consoleBox.getSelectionIndex() != -1)
        {
            replaceSelectedToken();
            consoleBox.setSelectionIndex(-1);
        }
        else
        {
            submitCommand(m_TypedLine);
            setTypedLine("", false);
        }
    }
    else if (glfwKey == GLFW_KEY_TAB)
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
            }
            else
            {
                if (consoleBox.getSelectionIndex() == -1)
                {
                    // nothing is selected -> select first element
                    consoleBox.setSelectionIndex(0);
                    if (mods & GLFW_MOD_SHIFT)
                        consoleBox.increaseSelectionIndex(-1);
                }
                else
                {
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
    if (!args.empty() && (m_History.empty() || m_History.back() != command))
        m_History.push_back(command);

    m_HistoryIndex = -1;
    m_PendingLine.clear();

    if (args.empty())
        return "";

    outputAdd(">> " + command);

    auto commandIterator = determineCommand(args);

    if (commandIterator != m_Commands.end())
    {
        std::string result;
        try
        {
            result = commandIterator->callback(args);
        }
        catch (const std::out_of_range& e)
        {
            result = "error: argument out of range";
        }
        catch (const std::invalid_argument& e)
        {
            result = "error: invalid argument";
        }
        outputAdd(result);
        return result;
    }

    outputAdd("-- Command not found --");
    return "NOTFOUND";
}

Logic::Console::Command& Console::registerCommand(const std::string& command, Callback callback)
{
    auto tokens = Utils::splitAndRemoveEmpty(command, ' ');
    std::vector<CandidateListGenerator> generators;
    auto simpleGen = [](std::string token) -> std::vector<Suggestion> {
        Suggestion suggestion = std::make_shared<SuggestionBase>(SuggestionBase{{token}});
        return {suggestion};
    };
    for (const auto& token : tokens)
    {
        generators.push_back(std::bind(simpleGen, token));
    }
    auto sanitizedCommand = Utils::join(tokens.begin(), tokens.end(), " ");
    bool requiresWorld = true;
    m_Commands.push_back({sanitizedCommand, generators, callback, generators.size(), requiresWorld});
    return m_Commands.back();
}

void Console::outputAdd(const std::string& msg)
{
    m_Output.push_front(msg);
}

std::list<Logic::Console::Command>::iterator Console::determineCommand(const std::vector<std::string>& tokens)
{
    bool worldAvailable = m_BaseEngine.getMainWorld().isValid();
    std::vector<std::size_t> numMatchingTokens(m_Commands.size(), 0);
    size_t commandIndex = 0;
    for (auto it = m_Commands.begin(); it != m_Commands.end(); ++it, ++commandIndex)
    {
        const auto& command = *it;
        if (!worldAvailable && command.requiresWorld)
            continue;
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
            numMatchingTokens[commandIndex] = command.numFixTokens;
        }
    }
    auto itMaxelement = std::max_element(numMatchingTokens.begin(), numMatchingTokens.end());
    if (itMaxelement != numMatchingTokens.end() && *itMaxelement != 0)
    {
        // case: at least one command matched with command.numFixTokens
        auto commandIndex = itMaxelement - numMatchingTokens.begin();
        return std::next(m_Commands.begin(), commandIndex);
    }
    return m_Commands.end();
}

/*
bool naturalComparator(const std::string& left, const std::string& right)
{
    // TODO
    return left < right;
}*/

void Console::generateSuggestions(const std::string& input, bool limitToFixed)
{
    using std::vector;
    using std::string;

    vector<string> tokens = tokenized(input);
    vector<vector<Suggestion>> suggestionsList;

    vector<bool> commandIsAlive;
    for (const auto& command : m_Commands)
    {
        bool disabled = command.requiresWorld && !m_BaseEngine.getMainWorld().isValid();
        commandIsAlive.push_back(!disabled);
    }

    // generate and evaluate suggestions
    for (std::size_t tokenID = 0; tokenID < tokens.size(); tokenID++)
    {
        const string& token = tokens[tokenID];
        auto tokenLowered = Utils::lowered(token);
        vector<Suggestion> matches;
        size_t cmdID = 0;
        for (auto commandIt = m_Commands.begin(); commandIt != m_Commands.end(); commandIt++, cmdID++)
        {
            auto& generators = commandIt->generators;
            std::size_t cmdEnd = limitToFixed ? commandIt->numFixTokens : generators.size();
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
            // filter out duplicates in consecutive groups
            std::sort(matches.begin(), matches.end(), [](const Suggestion& a, const Suggestion& b) -> bool {
                return *a < *b;
            });
            auto uniquePredicate = [](const Suggestion& a, const Suggestion& b) -> bool {
                return a->aliasList == b->aliasList;
            };
            matches.erase(std::unique(matches.begin(), matches.end(), uniquePredicate), matches.end());
            suggestionsList.push_back(std::move(matches));
        }
    }
    invalidateSuggestions();
    m_SuggestionsList = suggestionsList;
}

void Console::invalidateSuggestions()
{
    m_BaseEngine.getHud().getConsoleBox().setSelectionIndex(-1);
    m_SuggestionsList.clear();
}

std::vector<std::string> Console::tokenized(const std::string& line)
{
    std::vector<std::string> tokens = Utils::splitAndRemoveEmpty(line, ' ');
    if (tokens.empty() || isspace(line.back()))
    {
        // append empty pseudo token to trigger lookahead for the next token
        tokens.push_back("");
    }
    return tokens;
}

void Console::replaceSelectedToken()
{
    if (m_SuggestionsList.empty() || m_SuggestionsList.back().empty())
        return;
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

void Console::setTypedLine(const std::string& newLine, bool triggerSuggestions)
{
    m_TypedLine = newLine;
    invalidateSuggestions();
    if (triggerSuggestions)
        generateSuggestions(m_TypedLine, false);
}

void Console::setOpen(bool open)
{
    m_Open = open;
    if (!open)
        invalidateSuggestions();
}

using SuggestionBase = Logic::Console::SuggestionBase;

bool SuggestionBase::operator<(const SuggestionBase& b) const
{
    const SuggestionBase& a = *this;
    if (a.anyStartsWith != b.anyStartsWith)
        return !a.anyStartsWith < !b.anyStartsWith;
    return std::lexicographical_compare(a.aliasList.begin(), a.aliasList.end(),
                                        b.aliasList.begin(), b.aliasList.end());
}
