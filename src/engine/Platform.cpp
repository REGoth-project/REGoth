#include "Platform.h"
#include "utils/Utils.h"
#include <thread>

#include <bx/platform.h>
#include <bgfx/bgfxplatform.h>
#include <bgfx-cmake/bx/include/bx/commandline.h>



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

int Platform::m_WindowWidth;
int Platform::m_WindowHeight;

void Platform::windowSizeEvent(int width, int height)
{
    Input::windowSizeEvent(width, height);

    m_WindowHeight = height;
    m_WindowWidth = width;
}

int32_t Platform::run(int argc, char** argv)
{
    return 0;
}

void Platform::mainLoop(std::promise<int32_t>&& returnValue, int argc, char** argv)
{
    std::cout << "Initializing file reader/writer" << std::endl;
    Utils::initializeFileReaderWriter();

    std::cout << "Initializing engine...";
    init(argc, argv);

    std::cout << "Flushing bgfx";
    bgfx::frame();
}
