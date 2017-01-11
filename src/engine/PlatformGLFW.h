#pragma once
#include <GLFW/glfw3.h>
#include "Platform.h"
#include <future>

namespace Engine
{
    class PlatformGLFW : public Platform
    {
    public:
        int32_t run(int argc, char** argv) override;
        int shutdown() override;

    private:

        static void GLFWkeyEvent(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void GLFWmouseButtonEvent(GLFWwindow *window, int button, int action, int mods);
        static void GLFWmouseMoveEvent(GLFWwindow *window, double xPos, double yPos);
        static void GLFWscrollEvent(GLFWwindow *window, double xOffset, double yOffset);

        static void GLFWwindowSizeEvent(GLFWwindow *window, int width, int height);

        GLFWwindow* window;
    };
}
