

#include <bx/platform.h>
#if BX_PLATFORM_LINUX || BX_PLATFORM_OSX || BX_PLATFORM_WINDOWS || BX_PLATFORM_EMSCRIPTEN || BX_PLATFORM_BSD
#include <thread>
#include "PlatformGLFW.h"

// #include "utils/Utils.h"
namespace Utils
{
    void destroyFileReaderWriter();
}

#if defined(_glfw3_h_)
// If GLFW/glfw3.h is included before bgfx/platform.h we can enable GLFW3
// window interop convenience code.

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL
#elif BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif  //
#include <GLFW/glfw3native.h>
#endif
#include <bgfx-cmake/bx/include/bx/commandline.h>
#include <bgfx/platform.h>

//#include <bx/bx.h>
//#include <bgfx/bgfx.h>
//#include <bx/string.h>
//#include <bx/crtimpl.h>

//#include <time.h>

//#include <bx/thread.h>
//#include <bx/os.h>
//#include <bx/handlealloc.h>
//#include <string.h> // memset
//#include <string>

//#include <fcntl.h>

//#if BX_PLATFORM_EMSCRIPTEN
//#	include <emscripten.h>
//#endif // BX_PLATFORM_EMSCRIPTEN

//#include "entry_p.h"
//#include "cmd.h"
//#include "input.h"

//#define RMT_ENABLED ENTRY_CONFIG_PROFILER
//#include <remotery/lib/Remotery.h>

using namespace Engine;

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Platform* g_Platform = nullptr;

inline void glfwSetWindow(GLFWwindow* _window)
{
    bgfx::PlatformData pd;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    pd.ndt = glfwGetX11Display();
    pd.nwh = (void*)(uintptr_t)glfwGetGLXWindow(_window);
    pd.context = glfwGetGLXContext(_window);
#elif BX_PLATFORM_OSX
    pd.ndt = NULL;
    pd.nwh = glfwGetCocoaWindow(_window);
    pd.context = glfwGetNSGLContext(_window);
#elif BX_PLATFORM_WINDOWS
    pd.ndt = NULL;
    pd.nwh = glfwGetWin32Window(_window);
    pd.context = NULL;
#endif  // BX_PLATFORM_WINDOWS
    pd.backBuffer = NULL;
    pd.backBufferDS = NULL;
    setPlatformData(pd);
}

int32_t PlatformGLFW::run(int argc, char** argv)
{
    int width = 1280;
    int height = 720;

    std::cout << "Initializing GLFW" << std::endl;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    bx::CommandLine cmd(argc, (const char**)argv);

    bool fullscreen = false;
    if (cmd.hasArg('f'))
    {
        fullscreen = true;

        // Get native resolution
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        width = mode->width;
        height = mode->height;
    }

    GLFWwindow* window;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "REGoth", fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
    if (!window)
    {
        std::cout << "GLFW: Failed to create window!";
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
#if BX_PLATFORM_OSX
    glfwMakeContextCurrent(window);
#endif

    // Initialize bgfx
    glfwSetWindow(window);

    glfwSetCharCallback(window, GLFWCharEvent);
    glfwSetKeyCallback(window, GLFWkeyEvent);
    glfwSetMouseButtonCallback(window, GLFWmouseButtonEvent);
    glfwSetCursorPosCallback(window, GLFWmouseMoveEvent);
    glfwSetScrollCallback(window, GLFWscrollEvent);

    // window size is in screen units, while framebuffer size is in pixels
    // we want pixels
    glfwSetFramebufferSizeCallback(window, GLFWwindowSizeEvent);
    glfwGetFramebufferSize(window, &width, &height);
    GLFWwindowSizeEvent(window, width, height);

    setMouseLockCallback([&window](bool lock) {
        if (lock)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    });

    // TODO read bindings from file

    //    bindKey(GLFW_KEY_W, ActionType::DebugSkySpeed, false);

    // Camera Switch Keys

    std::cout << "Binding keys..." << std::endl;

    bindKey(GLFW_KEY_F1, ActionType::CameraThirdPerson, false);
    bindKey(GLFW_KEY_F2, ActionType::CameraFirstPerson, false);
    bindKey(GLFW_KEY_F3, ActionType::CameraFree, false);
    bindKey(GLFW_KEY_F4, ActionType::CameraViewer, false);

    // FirstPerson Camera Controller
    bindMouseAxis(MouseAxis::CursorX, ActionType::FirstPersonLookHorizontal, true, false);
    bindMouseAxis(MouseAxis::CursorY, ActionType::FirstPersonLookVertical, true, false);

    bindKey(GLFW_KEY_UP, ActionType::FirstPersonLookVertical, true, false);
    bindKey(GLFW_KEY_DOWN, ActionType::FirstPersonLookVertical, true, true);
    bindKey(GLFW_KEY_RIGHT, ActionType::FirstPersonLookHorizontal, true, false);
    bindKey(GLFW_KEY_LEFT, ActionType::FirstPersonLookHorizontal, true, true);

    // ThirdPerson Camera Controller
    bindMouseAxis(MouseAxis::CursorX, ActionType::ThirdPersonLookHorizontal, true, false);
    bindMouseAxis(MouseAxis::CursorY, ActionType::ThirdPersonLookVertical, true, false);
    bindMouseAxis(MouseAxis::ScrollY, ActionType::ThirdPersonMouseWheel, false);

    // Free Camera Controller
    bindKey(GLFW_KEY_W, ActionType::FreeMoveForward, true, false);
    bindKey(GLFW_KEY_S, ActionType::FreeMoveForward, true, true);
    bindKey(GLFW_KEY_D, ActionType::FreeMoveRight, true, false);
    bindKey(GLFW_KEY_A, ActionType::FreeMoveRight, true, true);
    bindKey(GLFW_KEY_Q, ActionType::FreeMoveUp, true, true);
    bindKey(GLFW_KEY_E, ActionType::FreeMoveUp, true, false);

    bindMouseAxis(MouseAxis::CursorX, ActionType::FreeLookHorizontal, true, false);
    bindMouseAxis(MouseAxis::CursorY, ActionType::FreeLookVertical, true, true);

    bindKey(GLFW_KEY_UP, ActionType::FreeLookVertical, true, false);
    bindKey(GLFW_KEY_DOWN, ActionType::FreeLookVertical, true, true);
    bindKey(GLFW_KEY_RIGHT, ActionType::FreeLookHorizontal, true, false);
    bindKey(GLFW_KEY_LEFT, ActionType::FreeLookHorizontal, true, true);

    // Viewer Camera Controller
    bindMouseAxis(MouseAxis::CursorX, ActionType::ViewerHorizontal, true);
    bindMouseAxis(MouseAxis::CursorY, ActionType::ViewerVertical, true);
    bindMouseButton(GLFW_MOUSE_BUTTON_MIDDLE, ActionType::ViewerRotate, true);
    bindMouseButton(GLFW_MOUSE_BUTTON_LEFT, ActionType::ViewerClick, false);
    bindKey(GLFW_KEY_LEFT_SHIFT, ActionType::ViewerPan, true);
    bindKey(GLFW_KEY_LEFT_CONTROL, ActionType::ViewerZoom, true);
    bindMouseAxis(MouseAxis::ScrollY, ActionType::ViewerMouseWheel, false);

    // Player Controller
    bindKey(GLFW_KEY_W, ActionType::PlayerForward, true);
    bindKey(GLFW_KEY_S, ActionType::PlayerBackward, true);
    bindKey(GLFW_KEY_LEFT, ActionType::PlayerTurnLeft, true);
    bindKey(GLFW_KEY_RIGHT, ActionType::PlayerTurnRight, true);
    bindKey(GLFW_KEY_UP, ActionType::PlayerForward, true);
    bindKey(GLFW_KEY_DOWN, ActionType::PlayerBackward, true);
    bindKey(GLFW_KEY_A, ActionType::PlayerStrafeLeft, true);
    bindKey(GLFW_KEY_D, ActionType::PlayerStrafeRight, true);
    bindKey(GLFW_KEY_Q, ActionType::PlayerTurnLeft, true);
    bindKey(GLFW_KEY_E, ActionType::PlayerTurnRight, true);
    bindKey(GLFW_KEY_1, ActionType::PlayerDrawWeaponMelee, true); // FIXME should be non-continous (also fix onAction)
    bindKey(GLFW_KEY_SPACE, ActionType::DebugMoveSpeed, true);
    bindKey(GLFW_KEY_LEFT_SHIFT, ActionType::DebugMoveSpeed2, true);
    bindKey(GLFW_KEY_LEFT_CONTROL, ActionType::PlayerAction, false);
    bindKey(GLFW_KEY_LEFT_CONTROL, ActionType::PlayerActionContinous, true);
    bindMouseButton(GLFW_MOUSE_BUTTON_LEFT, ActionType::PlayerAction, false);
    bindMouseButton(GLFW_MOUSE_BUTTON_LEFT, ActionType::PlayerActionContinous, true);
    bindKey(GLFW_KEY_LEFT_CONTROL, ActionType::PlayerAction, true);
    bindMouseAxis(MouseAxis::CursorX, ActionType::PlayerRotate, true, false);

    // Inventory bindings
    bindKey(GLFW_KEY_TAB, ActionType::Inventory_Show_Hide, false);
    bindKey(GLFW_KEY_UP, ActionType::Inventory_Up, false);
    bindKey(GLFW_KEY_DOWN, ActionType::Inventory_Down, false);
    bindKey(GLFW_KEY_LEFT, ActionType::Inventory_Left, false);
    bindKey(GLFW_KEY_RIGHT, ActionType::Inventory_Right, false);
    bindKey(GLFW_KEY_LEFT_CONTROL, ActionType::Inventory_Use, false);
    bindKey(GLFW_KEY_LEFT_ALT, ActionType::Inventory_Drop, false);

    bindKey(GLFW_KEY_F5, ActionType::Quicksave, false);
    bindKey(GLFW_KEY_F9, ActionType::Quickload, false);
    bindKey(GLFW_KEY_P, ActionType::PauseGame, false);

    bindKey(GLFW_KEY_ESCAPE, ActionType::UI_ToggleMainMenu, false);
    bindKey(GLFW_KEY_ESCAPE, ActionType::UI_Close, false);
    bindKey(GLFW_KEY_UP, ActionType::UI_Up, false);
    bindKey(GLFW_KEY_DOWN, ActionType::UI_Down, false);
    bindKey(GLFW_KEY_LEFT, ActionType::UI_Left, false);
    bindKey(GLFW_KEY_RIGHT, ActionType::UI_Right, false);
    bindKey(GLFW_KEY_W, ActionType::UI_Up, false);
    bindKey(GLFW_KEY_S, ActionType::UI_Down, false);
    bindKey(GLFW_KEY_A, ActionType::UI_Left, false);
    bindKey(GLFW_KEY_D, ActionType::UI_Right, false);
    bindKey(GLFW_KEY_ENTER, ActionType::UI_Confirm, false);
    bindKey(GLFW_KEY_LEFT_CONTROL, ActionType::UI_Confirm, false);
    bindKey(GLFW_KEY_0, ActionType::UI_0, false);
    bindKey(GLFW_KEY_1, ActionType::UI_1, false);
    bindKey(GLFW_KEY_2, ActionType::UI_2, false);
    bindKey(GLFW_KEY_3, ActionType::UI_3, false);
    bindKey(GLFW_KEY_4, ActionType::UI_4, false);
    bindKey(GLFW_KEY_5, ActionType::UI_5, false);
    bindKey(GLFW_KEY_6, ActionType::UI_6, false);
    bindKey(GLFW_KEY_7, ActionType::UI_7, false);
    bindKey(GLFW_KEY_8, ActionType::UI_8, false);
    bindKey(GLFW_KEY_9, ActionType::UI_9, false);
    bindKey(GLFW_KEY_F10, ActionType::UI_ToggleConsole, false);
    bindKey(GLFW_KEY_L, ActionType::UI_ToggleLogMenu, false);
    bindKey(GLFW_KEY_N, ActionType::UI_ToggleLogMenu, false);
    bindKey(GLFW_KEY_B, ActionType::UI_ToggleStatusMenu, false);
    bindKey(GLFW_KEY_BACKSPACE, ActionType::UI_ToggleStatusMenu, false);
    bindKey(GLFW_KEY_BACKSPACE, ActionType::UI_Backspace, false);
    bindKey(GLFW_KEY_HOME, ActionType::UI_HOME, false);
    bindKey(GLFW_KEY_END, ActionType::UI_END, false);
    bindMouseButton(GLFW_MOUSE_BUTTON_LEFT, ActionType::UI_Confirm, false);
    bindMouseButton(GLFW_MOUSE_BUTTON_RIGHT, ActionType::UI_Close, false);
    bindMouseAxis(MouseAxis::ScrollY, ActionType::UI_Mousewheel, false);

    //    // special keys test
    //    bindKey(GLFW_KEY_LEFT_SHIFT, ActionType::PlayerForward, true);
    //    bindKey(GLFW_KEY_RIGHT_SHIFT, ActionType::PlayerBackward, true);
    //    bindKey(GLFW_KEY_LEFT_CONTROL, ActionType::PlayerTurnLeft, true);
    //    bindKey(GLFW_KEY_RIGHT_CONTROL, ActionType::PlayerTurnRight, true);
    //    bindKey(GLFW_KEY_LEFT_ALT, ActionType::PlayerStrafeLeft, true);
    //    bindKey(GLFW_KEY_RIGHT_ALT, ActionType::PlayerStrafeRight, true);

    std::cout << "Starting logic-thread" << std::endl;

    // Start logic thread
    std::promise<int32_t> returnValue;

    mainLoop(std::move(returnValue), argc, argv);  // Not actually the mainloop. TODO: Rename method

#ifdef __EMSCRIPTEN__

    g_Platform = this;
    std::cout << "Running emscripten main-loop" << std::endl;
    emscripten_set_main_loop([]() { g_Platform->update(); }, 0, 1);
#else
    /* Loop until the user closes the window */
    while (true)
    {
        if (glfwWindowShouldClose(window))
        {
            // don't exit the loop immediately, give stuff a chance to check for quit
            setQuit(true);
        }

        /* Poll for and process events */
        glfwPollEvents();

        if (getQuit())
            break;

        update();
    }
#endif

    return 0;
}

int PlatformGLFW::shutdown()
{
    Utils::destroyFileReaderWriter();
    glfwTerminate();
    return 0;
}

void PlatformGLFW::GLFWkeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::keyEvent(key, scancode, action, mods);
}

void PlatformGLFW::GLFWmouseButtonEvent(GLFWwindow* window, int button, int action, int mods)
{
    Input::mouseButtonEvent(button, action, mods);
}

void PlatformGLFW::GLFWmouseMoveEvent(GLFWwindow* window, double xPos, double yPos)
{
    Input::mouseMoveEvent(xPos, yPos);
}

void PlatformGLFW::GLFWscrollEvent(GLFWwindow* window, double xOffset, double yOffset)
{
    Input::scrollEvent(xOffset, yOffset);
}

void PlatformGLFW::GLFWwindowSizeEvent(GLFWwindow* window, int width, int height)
{
    Input::windowSizeEvent(width, height);

    m_WindowHeight = static_cast<uint32_t>(height);
    m_WindowWidth = static_cast<uint32_t>(width);
}

std::string PlatformGLFW::getActualKeyName(int key)
{
    return std::string(glfwGetKeyName(key, 0));
}

void PlatformGLFW::GLFWCharEvent(GLFWwindow* window, unsigned int codepoint)
{
    PlatformGLFW::frameTextInput += codepoint;
}

#endif
