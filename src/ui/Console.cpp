#include <bgfx/bgfx.h>
#include <ZenLib/utils/split.h>
#include "Console.h"
#include <cctype>

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
            outputAdd(c.first);
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

    if(glfwKey >= Keys::PrintableBegin
       && glfwKey <= Keys::PrintableEnd)
    {
        m_TypedLine += std::tolower((char)glfwKey);
    }else if(glfwKey == Keys::GLFW_KEY_UP)
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
    }
}

std::string Console::submitCommand(const std::string& command)
{
    if((command.find_first_not_of(' ') != std::string::npos) && (m_History.empty() || m_History.back() != command))
        m_History.push_back(command);

    m_HistoryIndex = -1;
    m_PendingLine.clear();

    std::vector<std::string> args = Utils::split(command, ' ');

    outputAdd(" >> " + command);

    if(args.empty())
        return "";

    if(m_Commands.find(args[0]) != m_Commands.end())
    {

        std::string result = m_Commands[args[0]](args);

        outputAdd(result);

        return result;
    }

    outputAdd(" -- Command not found -- ");

    return "NOTFOUND";
}

void Console::registerCommand(const std::string& command,
                              CommandCallback callback)
{
    m_Commands[command] = callback;
}

void Console::registerAutocompleteFn(const std::string& command, Console::CommandCallback callback)
{
    m_AutocompleteCallbacks[command] = callback;
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


