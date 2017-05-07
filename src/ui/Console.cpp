#include <bgfx/bgfx.h>
#include <ZenLib/utils/split.h>
#include <utils/logger.h>
#include "Console.h"
#include <cctype>
#include <iterator>
#include <algorithm>
#include <utils/Utils.h>
#include <iomanip>
#include <engine/BaseEngine.h>

using namespace UI;

const uint16_t GLOBAL_Y = 25;

/* Function keys */
namespace Keys
{
    // All keys mapped to ascii-characters
    const int PrintableBegin = 32;
    const int PrintableEnd = 93; // Inclusive

    const int GLFW_KEY_ESCAPE = 256;
    const int GLFW_KEY_ENTER = 257;
    const int GLFW_KEY_TAB = 258;
    const int GLFW_KEY_BACKSPACE = 259;
    const int GLFW_KEY_INSERT = 260;
    const int GLFW_KEY_DELETE = 261;
    const int GLFW_KEY_RIGHT = 262;
    const int GLFW_KEY_LEFT = 263;
    const int GLFW_KEY_DOWN = 264;
    const int GLFW_KEY_UP = 265;
    const int GLFW_KEY_PAGE_UP = 266;
    const int GLFW_KEY_PAGE_DOWN = 267;
    const int GLFW_KEY_HOME = 268;
    const int GLFW_KEY_END = 269;
    const int GLFW_KEY_CAPS_LOCK = 280;
    const int GLFW_KEY_SCROLL_LOCK = 281;
    const int GLFW_KEY_NUM_LOCK = 282;
    const int GLFW_KEY_PRINT_SCREEN = 283;
    const int GLFW_KEY_PAUSE = 284;
    const int GLFW_KEY_F10 = 299;
};

Console::Console(Engine::BaseEngine& e) :
    m_BaseEngine(e),
    m_ConsoleBox(e, *this)
{
    m_HistoryIndex = 0;
    m_IsOpen = false;

    m_BaseEngine.getRootUIView().addChild(&m_ConsoleBox);
    outputAdd(" ----------- REGoth Console -----------");
}

Console::~Console() {
    m_BaseEngine.getRootUIView().removeChild(&m_ConsoleBox);
}

void Console::update()
{
}

void Console::onKeyDown(int glfwKey)
{
    // If this is Escape or F10, close/open the console
    if(glfwKey == Keys::GLFW_KEY_ESCAPE){
        setOpen(false);
    }
    if(glfwKey == Keys::GLFW_KEY_F10){
        setOpen(!isOpen());
    }
    if(glfwKey == Keys::GLFW_KEY_PAGE_DOWN)
    {
        m_ConsoleBox.increaseSelectionIndex(1);
    }
    if(glfwKey == Keys::GLFW_KEY_PAGE_UP)
    {
        m_ConsoleBox.increaseSelectionIndex(-1);
    }
    if(glfwKey == Keys::GLFW_KEY_HOME)
    {
        m_ConsoleBox.setSelectionIndex(0);
    }
    if(glfwKey == Keys::GLFW_KEY_END)
    {
        if (!m_SuggestionsList.empty())
        {
            m_ConsoleBox.setSelectionIndex(m_SuggestionsList.back().size() - 1);
        }
    }
    if(glfwKey == Keys::GLFW_KEY_UP)
    {
        const int historySize = m_History.size();
        if(historySize > m_HistoryIndex + 1)
        {
            if (m_HistoryIndex < 0)
                m_PendingLine = m_TypedLine;
            ++m_HistoryIndex;
            m_TypedLine = m_History.at(m_History.size() - m_HistoryIndex - 1);
            generateSuggestions(m_TypedLine, false);
        }
    }else if(glfwKey == Keys::GLFW_KEY_DOWN)
    {
        if(m_HistoryIndex >= 0)
        {
            --m_HistoryIndex;
            if (m_HistoryIndex < 0)
                m_TypedLine = m_PendingLine;
            else
                m_TypedLine = m_History.at(m_History.size() - m_HistoryIndex - 1);
            generateSuggestions(m_TypedLine, false);
        }
    }else if(glfwKey == Keys::GLFW_KEY_BACKSPACE)
    {
        if(m_TypedLine.size() >= 1)
        {
            m_TypedLine.pop_back();
            generateSuggestions(m_TypedLine, false);
        }
    }else if(glfwKey == Keys::GLFW_KEY_ENTER)
    {
        submitCommand(m_TypedLine);
        m_TypedLine.clear();
        generateSuggestions(m_TypedLine, false);
    }else if(glfwKey == Keys::GLFW_KEY_TAB)
    {
        if (m_SuggestionsList.empty())
        {
            generateSuggestions(m_TypedLine, false);
        }
        else if (!m_SuggestionsList.back().empty())
        {
            // default for now: only auto complete last token
            const auto& suggestions = m_SuggestionsList.back();
            auto bestSuggestion = suggestions.at(m_ConsoleBox.getSelectionIndex());
            std::vector<std::string> tokens = tokenized(m_TypedLine);
            tokens.back() = bestSuggestion->aliasList.at(bestSuggestion->bestAliasMatchIndex);
            std::string newLine = Utils::join(tokens.begin(), tokens.end(), " ");
            if (!tokens.empty())
            {
                newLine += " ";
            }
            m_TypedLine = newLine;
            generateSuggestions(m_TypedLine, false);
        }
    }
}

void Console::onTextInput(const std::string& text)
{
    if (!text.empty())
    {
        m_TypedLine += text;
        generateSuggestions(m_TypedLine, false);
    }
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

struct MatchInfo
{
    std::size_t pos;
    std::size_t caseMatches;
    std::size_t commandID;
    std::size_t groupID;
    std::string candidate;
    std::string candidateLowered;


    // smaller means better match
    static bool compare(const MatchInfo& a, const MatchInfo& b) {
        if (a.pos != b.pos){
            return a.pos < b.pos;
        } else if (a.caseMatches != b.caseMatches){
            return a.caseMatches > b.caseMatches;
        } else {
            return a.candidate < b.candidate;
        }
    };
};

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

using Suggestion = UI::ConsoleCommand::Suggestion;
void Console::generateSuggestions(const std::string& input, bool limitToFixed) {
    using std::vector;
    using std::string;

    vector<string> tokens = tokenized(input);

    vector<vector<Suggestion>> suggestionsList;
    vector<bool> commandIsAlive(m_Commands.size(), true);
    for (std::size_t tokenID = 0; tokenID < tokens.size(); tokenID++)
    {
        const string& token = tokens[tokenID];
        auto tokenLowered = Utils::lowered(token);
        vector<MatchInfo> matchInfos;
        vector<vector<Suggestion>> suggestionsByCommand(m_Commands.size());
        for (std::size_t cmdID = 0; cmdID < m_Commands.size(); cmdID++)
        {
            auto& command = m_Commands[cmdID];
            auto& generators = command.generators;
            std::size_t cmdEnd = limitToFixed ? command.numFixTokens : generators.size();
            if (commandIsAlive[cmdID] && tokenID < cmdEnd)
            {
                commandIsAlive[cmdID] = false;
                auto candidateGenerator = generators[tokenID];
                auto suggestions = candidateGenerator();
                for (std::size_t suggestionID = 0; suggestionID < suggestions.size(); suggestionID++)
                {
                    auto& suggestion = suggestions[suggestionID];
                    suggestionsByCommand[cmdID].push_back(suggestion);
                    auto& aliasList = suggestion->aliasList;
                    if (aliasList.empty())
                        continue;
                    std::vector<MatchInfo> groupInfos;
                    for (auto& candidate : aliasList)
                    {
                        // skip empty string alias
                        if (candidate.empty())
                            continue;
                        string candidateLowered = candidate;
                        Utils::lower(candidateLowered);
                        auto pos = candidateLowered.find(tokenLowered);
                        unsigned int caseMatches = 0;
                        for (std::size_t i = 0; i < token.size(); i++)
                        {
                            caseMatches += (token[i] == candidate[pos + i]);
                        }
                        MatchInfo matchInfo = MatchInfo{pos, caseMatches, cmdID, suggestionID, candidate, candidateLowered};
                        groupInfos.push_back(matchInfo);
                    }
                    if (!groupInfos.empty())
                    {
                        std::sort(groupInfos.begin(), groupInfos.end(), MatchInfo::compare);
                        auto& bestMatch = groupInfos.at(0);
                        if (bestMatch.pos != string::npos)
                        {
                            commandIsAlive[cmdID] = true;
                            matchInfos.push_back(bestMatch);
                            auto it = std::find(suggestion->aliasList.begin(), suggestion->aliasList.end(), bestMatch.candidate);
                            suggestion->bestAliasMatchIndex = it - suggestion->aliasList.begin();
                        }
                    }
                }
            }
        }
        // generate suggestions
        {
            vector<Suggestion> suggestionsForThisToken;
            std::set<string> known;
            std::sort(matchInfos.begin(), matchInfos.end(), MatchInfo::compare);
            for (const auto& matchInfo : matchInfos)
            {
                auto& suggestion = suggestionsByCommand[matchInfo.commandID][matchInfo.groupID];
                auto& aliasList = suggestion->aliasList;
                // filter out duplicates
                bool notInSet = known.insert(Utils::lowered(Utils::join(aliasList.begin(), aliasList.end(), ""))).second;
                if (notInSet)
                {
                    suggestionsForThisToken.push_back(suggestion);
                }
            }
            suggestionsList.push_back(suggestionsForThisToken);
        }
    }
    invalidateSuggestions();
    m_SuggestionsList = suggestionsList;
}

void Console::invalidateSuggestions() {
    m_ConsoleBox.setSelectionIndex(0);
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


