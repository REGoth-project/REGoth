#pragma once
#include "View.h"
#include <handle/HandleDef.h>

namespace UI
{
    class VideoView : public View
    {
    public:
        VideoView(Engine::BaseEngine& e);
        ~VideoView() override;

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config) override;

        /**
         * Enqueues a video to play inside the view
         */
        void enqueueVideo(const std::string& file);

    protected:

        /**
         * Plays a video inside the view
         */
        void playVideo(const std::string& file, Render::RenderConfig& config);

        /**
         * Full path of the played video file
         */
        std::string m_QueuedVideoFile;

        /**
         * Width and height of the given image
         */
        int32_t m_VideoWidth;
        int32_t m_VideoHeight;
    };
}
