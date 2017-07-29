#pragma once
#include <future>
#include "Input.h"

namespace Engine
{
    class Platform : public Input
    {
    public:
        virtual void init(int _argc, char** _argv) = 0;
        virtual int shutdown() = 0;
        virtual bool update() = 0;

        virtual int32_t run(int argc, char** argv) = 0;
        void mainLoop(std::promise<int32_t>&& returnValue, int argc, char** argv);

        /**
         * Accessors to windows height/width
         * @return height/width
         */
        static uint32_t getWindowWidth() { return m_WindowWidth; }
        static uint32_t getWindowHeight() { return m_WindowHeight; }
        static bool getQuit() { return m_Quit; }
        static void setQuit(bool quit) { m_Quit = quit; }
    protected:
        static void windowSizeEvent(int width, int height);

        static bool m_Quit;

        /**
         * Size of the current window
         */
        static uint32_t m_WindowWidth, m_WindowHeight;
    };
}
