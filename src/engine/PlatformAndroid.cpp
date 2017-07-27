//
// Created by desktop on 12.10.16.
//

#ifdef ANDROID

#include "PlatformAndroid.h"
#include <bgfx/bgfxplatform.h>
#include <android/input.h>
#include <utils/logger.h>

#define DBG(x) __android_log_print(ANDROID_LOG_INFO, "REGOTH", x)

const char* ARGS[] = {"android.so", "-g", "/sdcard/REGoth/Gothic", "-w", "world.zen"};
const int NUMARGS = 5;

const int ACTION_PlayerForward = 265;
const int ACTION_PlayerBackward = 264;
const int ACTION_PlayerTurnLeft = 263;
const int ACTION_PlayerTurnRight = 262;
const int ACTION_PlayerStrafeLeft = 5;
const int ACTION_PlayerStrafeRight = 6;
const int ACTION_DebugMoveSpeed = 7;
const int ACTION_DebugMoveSpeed2 = 8;
const int ACTION_PlayerAction = 257;

namespace bgfx
{
    ///
    inline void androidSetWindow(::ANativeWindow* _window)
    {
        PlatformData pd;
        pd.ndt = NULL;
        pd.nwh = _window;
        pd.context = NULL;
        pd.backBuffer = NULL;
        pd.backBufferDS = NULL;
        setPlatformData(pd);
    }

}  // namespace bgfx

using namespace Engine;

// Workaround for having main in target cpp
android_app* g_App = nullptr;

extern "C" {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <android_native_app_glue.c>
#pragma GCC diagnostic pop
}  // extern "C"

extern "C" int main(int argc, char** argv);
extern "C" void android_main(android_app* _app)
{
    app_dummy();  // Make sure glue isn't stripped

    __android_log_print(ANDROID_LOG_INFO, "REGOTH", "HolyShit you did it !");

    g_App = _app;

    const char* argv[] = {"android.so", "-g /sdcard/REGoth/Gothic", "-w world.zen"};
    main(3, const_cast<char**>(argv));
}

PlatformAndroid::PlatformAndroid()
{
    m_Window = nullptr;
    m_App = nullptr;

    m_ThumbstickPosition[0] = Math::float2(0, 0);
    m_ThumbstickPosition[1] = Math::float2(0, 0);
}

void PlatformAndroid::onAppCmd(struct android_app* app, int32_t cmd)
{
    DBG("On APP cmd called");

    struct EGL_Window* window = (struct EGL_Window*)app->userData;
    switch (cmd)
    {
        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state.  Do so.
            //window->m_pAppHandle->savedState = malloc(sizeof(saved_state));
            //*((struct saved_state*)window->m_pAppHandle->savedState) = engine->state;
            //window->m_pAppHandle->savedStateSize = sizeof(saved_state);
            break;
        case APP_CMD_INIT_WINDOW:
            // Command from main thread: a new ANativeWindow is ready for use.  Upon
            // receiving this command, android_app->window will contain the new window
            // surface.
            if (m_Window == NULL)
            {
                DBG("Setting window");
                m_Window = m_App->window;
                bgfx::androidSetWindow(m_Window);

                int32_t width = ANativeWindow_getWidth(m_Window);
                int32_t height = ANativeWindow_getHeight(m_Window);

                //DBG("ANativeWindow width %d, height %d", width, height);
                windowSizeEvent(width, height);

                m_Thread = std::thread(
                    [this]() {
                        std::promise<int32_t> returnValue;
                        mainLoop(std::move(returnValue), NUMARGS, const_cast<char**>(ARGS));
                        while (true)
                        {
                            update();
                        }
                    });
                //m_thread.init(MainThreadEntry::threadFunc, &m_mte);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            //engine_term_display(window);
            setQuit(true);
            break;
        case APP_CMD_GAINED_FOCUS:
            break;
        case APP_CMD_LOST_FOCUS:
            break;
    }
}

int PlatformAndroid::onInputEvent(struct android_app* app, AInputEvent* event)
{
    static int activePointerIdx;

    LogInfo() << "Input event!";

    switch (AInputEvent_getSource(event))
    {
        case AINPUT_SOURCE_TOUCHSCREEN:
            int action = AKeyEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
            int pointer = (AKeyEvent_getAction(event) & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

            LogInfo() << "Pointer: " << AMotionEvent_getPointerId(event, pointer);

            // Only need two pointers for now
            //if(pointer > 1)
            //	break;
            static int s_tumbLastFrameDown[2] = {0, 0};

            switch (action)
            {
                case AMOTION_EVENT_ACTION_MOVE:
                case AMOTION_EVENT_ACTION_DOWN:
                case AMOTION_EVENT_ACTION_POINTER_DOWN:
                {
                    int pointerCount = AMotionEvent_getPointerCount(event);
                    for (int i = 0; i < pointerCount; ++i)
                    {
                        int pointerIndex = i;
                        int x = AMotionEvent_getX(event, pointerIndex);
                        int y = AMotionEvent_getY(event, pointerIndex);

                        const Math::float2 controlSize(getWindowWidth() / 16, getWindowHeight() / 16);

                        mouseMoveEvent(x, y);
                        mouseButtonEvent(0, KEY_ACTION_PRESS, 0);

                        // Movement
                        int movementX = 0;
                        int movementY = 0;
                        int thumb = 0;

                        if (y > getWindowHeight() / 2)
                        {
                            if (x < getWindowWidth() / 2)
                            {
                                // Left stick
                                movementX = getWindowWidth() / 6;
                                movementY = getWindowHeight() - (getWindowHeight() / 6);
                                thumb = 0;

                                Math::float2 dir = Math::float2(static_cast<float>(x - movementX), static_cast<float>(y - movementY));

                                LogInfo() << "Thumb Source" << thumb << ": " << dir.toString();

                                //  Set max thumbstick position

                                dir.x = dir.x > 0 ? std::min(controlSize.x, dir.x) : std::max(-controlSize.x, dir.x);
                                dir.y = dir.y > 0 ? std::min(controlSize.y, dir.y) : std::max(-controlSize.y, dir.y);

                                // Normalize
                                dir.x /= controlSize.length();
                                dir.y /= controlSize.length();

                                LogInfo() << "Thumb " << thumb << ": " << dir.toString();

                                m_ThumbstickPosition[0] = dir;
                            }
                            else
                            {
                                // Right stick
                                movementX = x - (getWindowWidth() - (getWindowWidth() / 6));
                                movementY = y - (getWindowWidth() - (getWindowHeight() / 6));
                                thumb = 1;

                                static int s_lastX = movementX;
                                static int s_lastY = movementY;

                                if (!s_tumbLastFrameDown[1])
                                {
                                    s_lastX = movementX;
                                    s_lastY = movementY;
                                }

                                m_ThumbstickPosition[1] = 0.2f * Math::float2(static_cast<float>(movementX - s_lastX), static_cast<float>(movementY - s_lastY));

                                s_lastX = movementX;
                                s_lastY = movementY;

                                LogInfo() << "Thumb " << thumb << ": " << m_ThumbstickPosition[1].toString();

                                s_tumbLastFrameDown[1] = 1;
                            }
                        }
                        else
                        {
                            keyEvent(ACTION_PlayerAction, 0, KEY_ACTION_PRESS, 0);
                        }
                    }
                }
                    return 1;
                    break;
                case AMOTION_EVENT_ACTION_UP:
                case AMOTION_EVENT_ACTION_POINTER_UP:
                {
                    int thumb = 0;
                    int x = AMotionEvent_getX(event, pointer);
                    int y = AMotionEvent_getY(event, pointer);

                    if (x < getWindowWidth() / 2)
                        thumb = 0;
                    else
                        thumb = 1;

                    s_tumbLastFrameDown[thumb] = 0;
                    m_ThumbstickPosition[thumb] = Math::float2(0, 0);

                    mouseButtonEvent(0, KEY_ACTION_RELEASE, 0);
                    keyEvent(ACTION_PlayerAction, 0, KEY_ACTION_RELEASE, 0);
                }
                    return 1;
                    break;
            }
            break;
    }  // end switch
}

int32_t PlatformAndroid::run(int argc, char** argv)
{
    m_App = g_App;
    assert(m_App);

    DBG("Inside RUN");

    m_App->userData = this;
    m_App->onAppCmd = [](struct android_app* app, int32_t cmd) {
        PlatformAndroid* pl = (PlatformAndroid*)app->userData;
        pl->onAppCmd(app, cmd);
    };

    m_App->onInputEvent = [](struct android_app* app, AInputEvent* event) {
        PlatformAndroid* pl = (PlatformAndroid*)app->userData;
        return pl->onInputEvent(app, event);
    };

    bindKey(ACTION_PlayerForward, ActionType::PlayerForward, true);
    bindKey(ACTION_PlayerBackward, ActionType::PlayerBackward, true);
    bindKey(ACTION_PlayerTurnLeft, ActionType::PlayerTurnLeft, true);
    bindKey(ACTION_PlayerTurnRight, ActionType::PlayerTurnRight, true);
    bindKey(ACTION_PlayerStrafeLeft, ActionType::PlayerStrafeLeft, true);
    bindKey(ACTION_PlayerStrafeRight, ActionType::PlayerStrafeRight, true);
    bindKey(ACTION_PlayerAction, ActionType::PlayerAction, false);

    // Read all pending events.
    int ident;
    int events;
    struct android_poll_source* source;

    while (0 == m_App->destroyRequested)
    {
        int dirPressed = 0;

        // Map directions to keys
        if (m_ThumbstickPosition[0] != Math::float2(0, 0))
        {
            if (m_ThumbstickPosition[0].dot(Math::float2(0.0f, 1.0f)) > 0.5f)
                dirPressed = ACTION_PlayerBackward;
            else if (m_ThumbstickPosition[0].dot(Math::float2(0.0f, -1.0f)) > 0.5f)
                dirPressed = ACTION_PlayerForward;
            else if (m_ThumbstickPosition[0].dot(Math::float2(-1.0f, 0.0f)) > 0.5f)
                dirPressed = ACTION_PlayerStrafeLeft;
            else if (m_ThumbstickPosition[0].dot(Math::float2(1.0f, 0.0f)) > 0.5f)
                dirPressed = ACTION_PlayerStrafeRight;
        }

        keyEvent(ACTION_PlayerForward, 0, KEY_ACTION_RELEASE, 0);
        keyEvent(ACTION_PlayerBackward, 0, KEY_ACTION_RELEASE, 0);
        keyEvent(ACTION_PlayerTurnLeft, 0, KEY_ACTION_RELEASE, 0);
        keyEvent(ACTION_PlayerTurnRight, 0, KEY_ACTION_RELEASE, 0);
        keyEvent(ACTION_PlayerStrafeLeft, 0, KEY_ACTION_RELEASE, 0);
        keyEvent(ACTION_PlayerStrafeRight, 0, KEY_ACTION_RELEASE, 0);

        if (dirPressed > 0)
            keyEvent(dirPressed, 0, KEY_ACTION_PRESS, 0);

        if (m_ThumbstickPosition[1].x > 0)
            keyEvent(ACTION_PlayerTurnRight, 0, KEY_ACTION_PRESS, 0);
        else if (m_ThumbstickPosition[1].x < 0)
            keyEvent(ACTION_PlayerTurnLeft, 0, KEY_ACTION_PRESS, 0);

        /*while ((ident=ALooper_pollAll(0, NULL, &events,
        (void**)&source)) >= 0) {*/

        int32_t num;
        ALooper_pollAll(-1, NULL, &num, (void**)&source);
        // Process this event.
        if (source != NULL)
        {
            source->process(m_App, source);
        }
    }

    m_Thread.join();

    return 0;
}

#endif
