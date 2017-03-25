#include <bgfx/bgfx.h>
#include <ZenLib/utils/split.h>
#include <utils/logger.h>
#include "Console.h"
#include <cctype>
#include <iterator>
#include <algorithm>
#include <utils/Utils.h>

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
        for(auto& c : m_Commands)
            outputAdd(c);
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

std::string Console::submitCommand(const std::string& command)
{
    if((command.find_first_not_of(' ') != std::string::npos) && (m_History.empty() || m_History.back() != command))
        m_History.push_back(command);

    m_HistoryIndex = -1;
    m_PendingLine.clear();

    outputAdd(" >> " + command);

    if(command.empty())
        return "";

    auto commandAutoCompleted = command;
    auto commID = autoComplete(commandAutoCompleted, false, false, true);

    outputAdd(" >> (Auto) " + commandAutoCompleted);

    size_t bestMatchSize = 0;
    int bestMatchIndex = -1;
    for (size_t i = 0; i < m_Commands.size(); ++i)
    {
        const std::string &candidate = m_Commands.at(i);
        if (candidate.size() < bestMatchSize) {
            // We already found a better command candidate
            continue;
        }

        if (command.size() == candidate.size() || (command.size() > candidate.size() && command.at(candidate.size()) == ' '))
        {
            // it makes sense to compare
            if (candidate == command.substr(0, candidate.size()))
            {
                // Match!
                bestMatchSize = candidate.size();
                bestMatchIndex = i;
            }
        }
    }

    if (bestMatchIndex >= 0)
    {
        const std::vector<std::string> args = Utils::split(command, ' ');
        std::string result;
        try {
            result = m_CommandCallbacks.at(bestMatchIndex)(args);
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

void Console::registerCommand(const std::string& command,
                              ConsoleCommand::Callback callback)
{
    m_Commands.push_back(command);
    m_CommandCallbacks.push_back(callback);
}

void Console::registerCommand2(std::vector<std::function<std::vector<std::string>()>> generators,
                               ConsoleCommand::Callback callback,
                               unsigned int numFixTokens)
{
    m_Commands2.emplace_back(ConsoleCommand{generators, callback, numFixTokens});
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

struct MatchInfo{
    std::size_t notMatchingCharCount;
    std::size_t commandID;
    std::string candidate;
};

int Console::autoComplete(std::string& input, bool limitToFixed, bool showSuggestions, bool overwriteInput) {
    using std::vector;
    using std::string;

    auto lineLower = input;
    Utils::lower(lineLower);
    std::istringstream iss(lineLower);
    vector<string> tokens;
    std::copy(std::istream_iterator<string>(iss), std::istream_iterator<string>(), std::back_inserter(tokens));

    vector<std::tuple<string, bool>> newTokens;
    for (auto& token : tokens)
        newTokens.push_back(std::make_tuple(token, false));

    if (tokens.empty())
        return -1;

    vector<bool> commandIsAlive(m_Commands2.size(), true);
    for (std::size_t tokenID = 0; tokenID < tokens.size(); tokenID++)
    {
        auto& token = tokens[tokenID];
        vector<MatchInfo> matchInfosStartsWith;
        vector<MatchInfo> matchInfosInMiddle;
        for (std::size_t cmdID = 0; cmdID < m_Commands2.size(); cmdID++)
        {
            auto& command = m_Commands2[cmdID];
            auto& generators = command.generators;
            std::size_t cmdEnd = limitToFixed ? command.numFixTokens : generators.size();
            if (commandIsAlive[cmdID] && tokenID < cmdEnd)
            {
                commandIsAlive[cmdID] = false;
                auto candidateGenerator = generators[tokenID];
                auto candidates = candidateGenerator();
                for (auto& candidate : candidates)
                {
                    string candidateLowered = candidate;
                    Utils::lower(candidateLowered);
                    std::remove_if(candidateLowered.begin(), candidateLowered.end(), isspace);
                    auto pos = candidateLowered.find(token);
                    auto diff = candidateLowered.size() - token.size();
                    if (pos == 0)
                    {
                        matchInfosStartsWith.emplace_back(MatchInfo{diff, cmdID, candidateLowered});
                    } else if (pos != string::npos)
                    {
                        matchInfosInMiddle.emplace_back(MatchInfo{diff, cmdID, candidateLowered});
                    }
                }
            }
        }
        vector<MatchInfo> suggestions;
        for (auto& matchInfos : {matchInfosStartsWith, matchInfosInMiddle})
        {
            if (matchInfos.empty())
                continue;

            string reference = matchInfos.front().candidate;
            std::size_t commonLength = reference.size();
            auto longestCandidateLen = reference.size();
            for (auto& matchInfo : matchInfos)
            {
                suggestions.push_back(matchInfo);
                commandIsAlive[matchInfo.commandID] = true;
                commonLength = std::min(Utils::commonStartLength(reference, matchInfo.candidate), commonLength);
                longestCandidateLen = std::max(longestCandidateLen, matchInfo.candidate.size());
            }
            if (commonLength != 0)
            {
                string tokenNew = reference.substr(0, commonLength);
                bool thereIsNoLongerCandidate = longestCandidateLen == tokenNew.size();
                newTokens[tokenID] = std::make_tuple(tokenNew, thereIsNoLongerCandidate);
            }
            break;
        }
        if (showSuggestions)
        {
            std::stringstream ss;
            auto sorter = [](const MatchInfo& a, const MatchInfo& b) -> bool {
                return a.notMatchingCharCount < b.notMatchingCharCount;
            };
            std::sort(suggestions.begin(), suggestions.end(), sorter);
            for (auto& matchInfo : suggestions)
            {
                ss << matchInfo.candidate << " ";
            }
            LogInfo() << "suggestions: " << ss.str();
        }
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
    if (std::count(commandIsAlive.begin(), commandIsAlive.end(), true) == 1)
    {
        auto comIt = std::find(commandIsAlive.begin(), commandIsAlive.end(), true);
        auto commID = comIt - commandIsAlive.begin();
        return static_cast<int>(commID);
    } else {
        return -1;
    }
}




