#pragma once
#include "bgfx_utils.h"
#include <engine/GameEngine.h>
#include <engine/World.h>
#include <utils/bgfx_lib.h>
#if BX_PLATFORM_ANDROID
#include "engine/PlatformAndroid.h"
using REGothPlatform = Engine::PlatformAndroid;
#elif BX_PLATFORM_LINUX || BX_PLATFORM_OSX || BX_PLATFORM_WINDOWS || BX_PLATFORM_EMSCRIPTEN || BX_PLATFORM_BSD
#include "engine/PlatformGLFW.h"
using REGothPlatform = Engine::PlatformGLFW;
#else
#error Unknown platform
#endif

class REGoth : public REGothPlatform
{
public:
    void init(int _argc, char** _argv) override;
    void initConsole();
    int shutdown() override;
    bool update() override;
    void drawLog();
    void showSplash();
    void renderScreenSpaceQuad(uint32_t _view, bgfx::ProgramHandle _program, float _x, float _y, float _width, float _height);

    Engine::GameEngine* m_pEngine;
    uint32_t m_debug;
    uint32_t m_reset;
    uint32_t m_Width, m_Height;
    int64_t m_timeOffset;
    int32_t m_scrollArea;
    bool m_NoHUD;
    // prevents imgui from crashing if we failed on startup and didn't init it
    bool m_ImgUiCreated = false;
};
