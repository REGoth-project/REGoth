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
};

Console::Console()
{
    m_Config.height = 10;
}

void Console::update()
{
    bgfx::dbgTextPrintf(0, (uint16_t)(GLOBAL_Y + m_Config.height + 1), 0x4f, "> %s", m_TypedLine.c_str());
    printHistory();
}

void Console::onKeyDown(int glfwKey)
{
    if(glfwKey >= Keys::PrintableBegin
       && glfwKey <= Keys::PrintableEnd)
    {
        m_TypedLine += std::tolower((char)glfwKey);
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
    std::vector<std::string> args = Utils::split(command, ' ');

    historyAdd(" >> " + command);

    if(m_Commands.find(args[0]) != m_Commands.end())
    {

        std::string result = m_Commands[args[0]](args);

        historyAdd(result);

        return result;
    }

    historyAdd(" -- Command not found -- ");

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

void Console::printHistory()
{
    int i=0;
    for(const std::string& s : m_History)
    {
        if(i == m_Config.height)
            break;

        bgfx::dbgTextPrintf(0, (uint16_t)(GLOBAL_Y + m_Config.height - i), 0x0f, "| %s", s.c_str());

        i++;
    }
}

void Console::historyAdd(const std::string& msg)
{
    m_History.push_front(msg);
}


