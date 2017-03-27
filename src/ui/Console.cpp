#include <bgfx/bgfx.h>
#include <ZenLib/utils/split.h>
#include <utils/logger.h>
#include "Console.h"
#include <cctype>
#include <iterator>
#include <algorithm>
#include <utils/Utils.h>
#include <iomanip>

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

Console::Console()
{
    m_Config.height = 10;
    m_HistoryIndex = 0;
    m_IsOpen = false;
    outputAdd(" ----------- REGoth Console -----------");

    registerCommand("list", [this](const std::vector<std::string>& args) -> std::string {
        for(auto& command : m_Commands)
        {
            outputAdd(command.commandName);
        }
        return "";
    });
}

void Console::update()
{
    bgfx::dbgTextPrintf(0, (uint16_t)(GLOBAL_Y + m_Config.height + 1), 0x4f, "> %s", m_TypedLine.c_str());
    printOutput();
}

void Console::onKeyDown(int glfwKey)
{
    // If this is Escape or F10, close/open the console
    if(glfwKey == Keys::GLFW_KEY_ESCAPE)
        setOpen(false);

    if(glfwKey == Keys::GLFW_KEY_F10)
        setOpen(!isOpen());

    if(glfwKey == Keys::GLFW_KEY_UP)
    {
        const int historySize = m_History.size();
        if(historySize > m_HistoryIndex + 1)
        {
            if (m_HistoryIndex < 0)
                m_PendingLine = m_TypedLine;
            ++m_HistoryIndex;
            m_TypedLine = m_History.at(m_History.size() - m_HistoryIndex - 1);
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
        }
    }else if(glfwKey == Keys::GLFW_KEY_BACKSPACE)
    {
        if(m_TypedLine.size() >= 1)
            m_TypedLine.pop_back();
    }else if(glfwKey == Keys::GLFW_KEY_ENTER)
    {
        submitCommand(m_TypedLine);
        m_TypedLine.clear();
    }else if(glfwKey == Keys::GLFW_KEY_TAB)
    {
        autoComplete(m_TypedLine, false, true, true);
    }
}

void Console::onTextInput(const std::string& text)
{
    m_TypedLine += text;
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
    // autoComplete(command, autoCompleteCommandTokensOnly, false, true);

    outputAdd(" >> " + command);

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

    outputAdd(" -- Command not found -- ");
    return "NOTFOUND";
}

ConsoleCommand& Console::registerCommand(const std::string& command, ConsoleCommand::Callback callback)
{
    auto tokens = Utils::splitAndRemoveEmpty(command, ' ');
    std::vector<ConsoleCommand::CandidateListGenerator> generators;
    auto simpleGen = [](std::string token) -> std::vector<std::vector<std::string>> {
        return {{token}};
    };
    for (auto tokenIt = tokens.begin(); tokenIt != tokens.end(); tokenIt++)
    {
        std::string token = *tokenIt;
        generators.push_back(std::bind(simpleGen, token));
    }
    auto sanitizedCommand = Utils::join(tokens.begin(), tokens.end(), " ");
    m_Commands.emplace_back(ConsoleCommand{sanitizedCommand, generators, callback, generators.size()});
    return m_Commands.back();
}

void Console::printOutput()
{
    int i=0;
    for(const std::string& s : m_Output)
    {
        if(i == m_Config.height)
            break;

        bgfx::dbgTextPrintf(0, (uint16_t)(GLOBAL_Y + m_Config.height - i), 0x0f, "| %s", s.c_str());

        i++;
    }
}

void Console::outputAdd(const std::string& msg)
{
    m_Output.push_front(msg);
}

struct MatchInfo
{
    std::size_t pos;
    std::size_t notMatchingCharCount;
    std::size_t commandID;
    std::size_t groupID;
    std::string candidate;
    std::string candidateLowered;

    static bool compare(const MatchInfo& a, const MatchInfo& b) {
        if (a.pos != b.pos){
            return a.pos < b.pos;
        } else {
            return a.notMatchingCharCount < b.notMatchingCharCount;
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
            auto group = Utils::findNameInGroups(command.generators.at(tokenID)(), tokens.at(tokenID));
            if (group.empty())
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

void Console::autoComplete(std::string& input, bool limitToFixed, bool showSuggestions, bool overwriteInput) {
    using std::vector;
    using std::string;

    vector<string> tokens = Utils::splitAndRemoveEmpty(input, ' ');

    bool lookAhead = false;
    if (tokens.empty() || isspace(input.back()))
    {
        // append empty pseudo token to trigger lookahead for the next token
        tokens.push_back("");
        lookAhead = true;
    }

    vector<std::tuple<string, bool>> newTokens;
    for (auto& token : tokens)
    {
        newTokens.push_back(std::make_tuple(token, false));
    }

    vector<bool> commandIsAlive(m_Commands.size(), true);
    for (std::size_t tokenID = 0; tokenID < tokens.size(); tokenID++)
    {
        auto tokenLowered = Utils::lowered(tokens[tokenID]);
        vector<MatchInfo> matchInfosStartsWith;
        vector<MatchInfo> matchInfosInMiddle;
        vector<vector<vector<string>>> allGroups(m_Commands.size());
        for (std::size_t cmdID = 0; cmdID < m_Commands.size(); cmdID++)
        {
            auto& command = m_Commands[cmdID];
            auto& generators = command.generators;
            std::size_t cmdEnd = limitToFixed ? command.numFixTokens : generators.size();
            if (commandIsAlive[cmdID] && tokenID < cmdEnd)
            {
                commandIsAlive[cmdID] = false;
                auto candidateGenerator = generators[tokenID];
                auto groups = candidateGenerator();
                for (std::size_t groupID = 0; groupID < groups.size(); groupID++)
                {
                    auto& aliasGroup = groups[groupID];
                    allGroups[cmdID].push_back(aliasGroup);
                    if (aliasGroup.empty())
                        continue;
                    std::vector<MatchInfo> groupInfos;
                    for (auto& candidate : aliasGroup)
                    {
                        string candidateLowered = candidate;
                        Utils::lower(candidateLowered);
                        auto pos = candidateLowered.find(tokenLowered);
                        auto diff = candidateLowered.size() - tokenLowered.size();
                        MatchInfo matchInfo = MatchInfo{pos, diff, cmdID, groupID, candidate, candidateLowered};
                        groupInfos.push_back(matchInfo);
                    }
                    std::sort(groupInfos.begin(), groupInfos.end(), MatchInfo::compare);
                    auto& bestMatch = groupInfos.at(0);
                    if (bestMatch.pos == 0)
                    {
                        matchInfosStartsWith.push_back(bestMatch);
                    } else if (bestMatch.pos != string::npos)
                    {
                        matchInfosInMiddle.push_back(bestMatch);
                    }
                }
            }
        }
        for (auto& matchInfos : {matchInfosStartsWith, matchInfosInMiddle})
        {
            if (matchInfos.empty())
                continue;

            string reference = matchInfos.front().candidateLowered;
            std::size_t commonLength = reference.size();
            auto longestCandidateLen = reference.size();
            for (auto& matchInfo : matchInfos)
            {
                commandIsAlive[matchInfo.commandID] = true;
                commonLength = std::min(Utils::commonStartLength(reference, matchInfo.candidateLowered), commonLength);
                longestCandidateLen = std::max(longestCandidateLen, matchInfo.candidateLowered.size());
            }
            if (commonLength != 0)
            {
                string tokenNew = matchInfos.front().candidate.substr(0, commonLength);
                bool thereIsNoLongerCandidate = longestCandidateLen == tokenNew.size();
                newTokens[tokenID] = std::make_tuple(tokenNew, thereIsNoLongerCandidate);
            }
            break;
        }
        if (showSuggestions)
        {
            vector<vector<string>> suggestions;
            LogInfo() << "suggestions:";
            for (auto matchInfos : {&matchInfosStartsWith, &matchInfosInMiddle})
            {
                std::sort(matchInfos->begin(), matchInfos->end(), MatchInfo::compare);
                for (auto& matchInfo : *matchInfos)
                {
                    suggestions.push_back({});
                    for (auto& alias : allGroups[matchInfo.commandID][matchInfo.groupID])
                    {
                        suggestions.back().push_back(alias);
                    }
                }
            }
            std::size_t maxSuggestions = 15;
            for (auto& suggestionEntry : suggestions)
            {
                if (maxSuggestions-- == 0)
                {
                    LogInfo() << "... (suggestion maximum reached)";
                    break;
                }
                std::stringstream ss;
                for (auto& alias : suggestionEntry)
                {
                    ss << std::left << std::setw(40) << alias;
                }
                LogInfo() << ss.str();
            }
        }
    }
    if (lookAhead && std::get<0>(newTokens.back()) == "")
    {
        tokens.pop_back();
        newTokens.pop_back();
    }
    if (overwriteInput)
    {
        std::stringstream tokenConCat;
        for (auto it = newTokens.begin(); it != newTokens.end(); it++)
        {
            bool noLongerMatch = std::get<1>(*it);
            tokenConCat << std::get<0>(*it);
            if (it != newTokens.end() - 1 || isspace(input.back()) || noLongerMatch)
                tokenConCat << " ";
        }
        input = tokenConCat.str();
    }
}

