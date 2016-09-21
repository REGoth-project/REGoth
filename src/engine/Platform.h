#pragma once
#include <GLFW/glfw3.h>
#include "Input.h"
#include <future>

namespace Engine
{

class Platform : public Input
{
public:
    virtual void init(int _argc, char** _argv) = 0;
    virtual int  shutdown() = 0;
    virtual bool update() = 0;

    int32_t run(int argc, char** argv);
    void mainLoop(std::promise<int32_t> && returnValue, int argc, char** argv);

    /**
     * Accessors to windows height/width
     * @return height/width
     */
    static int getWindowWidth(){ return m_WindowWidth; }
    static int getWindowHeight(){ return m_WindowHeight; }
private:

    static void windowSizeEvent(GLFWwindow *window, int width, int height);

    GLFWwindow* window;

    /**
     * Size of the current window
     */
    static int m_WindowWidth, m_WindowHeight;
};
}
