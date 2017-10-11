#pragma once
#include <future>
#include "Platform.h"
#include <GLFW/glfw3.h>

namespace Engine
{
    class PlatformGLFW : public Platform
    {
    public:
        int32_t run(int argc, char** argv) override;
        int shutdown() override;

        /**
         * Looks up the actual key to the given scancode. Libraries like GLFW work with mapping everything to a US-Layout,
         * which means that pressing the key that is on the location of the US Y key, for example, will always result in the
         * scancode of Y to be returned.
         * That is a problem on QWERTZ or AZERTY keyboards. This function returns a sequence of chars
         * which contain the name of the actual pressed key.
         * @param scancode Scancode from input-library (GLFW)
         * @return Actual name of the pressed key
         */
        std::string getActualKeyName(int scancode) override;

    private:
        static void GLFWkeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void GLFWCharEvent(GLFWwindow* window, unsigned int codepoint);
        static void GLFWmouseButtonEvent(GLFWwindow* window, int button, int action, int mods);
        static void GLFWmouseMoveEvent(GLFWwindow* window, double xPos, double yPos);
        static void GLFWscrollEvent(GLFWwindow* window, double xOffset, double yOffset);

        static void GLFWwindowSizeEvent(GLFWwindow* window, int width, int height);

        GLFWwindow* window;
    };
}
