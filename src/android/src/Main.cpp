#include <android_native_app_glue.h>
#include <android/log.h>

#define APPNAME "TestApp"

void android_main(struct android_app* state)
{
    app_dummy();  // Make sure glue isn't stripped

    __android_log_print(ANDROID_LOG_INFO, APPNAME, "HolyShit you did it !");

    ANativeActivity_finish(state->activity);
}
