#pragma once

#ifdef ANDROID

#include <future>
#include "Platform.h"

#include <android_native_app_glue.h>
#include <android/input.h>
#include <android/log.h>
#include <android/looper.h>
#include <android/window.h>

namespace Engine
{
    class PlatformAndroid : public Platform
    {
    public:
        PlatformAndroid();

        int32_t run(int argc, char** argv) override;

    private:
        void onAppCmd(struct android_app* app, int32_t cmd);
        int onInputEvent(struct android_app* app, AInputEvent* event);

        android_app* m_App;
        ANativeWindow* m_Window;
        std::thread m_Thread;

        /**
		 * @brief Virtual onscreen-thumbstick
		 */
        Math::float2 m_ThumbstickPosition[2];
    };
}

#endif