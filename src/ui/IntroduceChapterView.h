
#pragma once

#include "View.h"
#include "ImageView.h"
#include "TextView.h"

namespace UI
{
    class IntroduceChapterView : public View
    {
    public:
        IntroduceChapterView(Engine::BaseEngine& e);

        ~IntroduceChapterView();

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config) override;

        /**
         * Adds a single choice
         * @param choice Choice presented to the user
         * @param name Name of the person or thing we are interacting with
         */
        void enqueueChapterIntroduction(std::string title, std::string subtitle, std::string texture_name, std::string sound_name, double wait_time);

    private:

        void initializeIntroduceChapterView();
        
        enum QueueStatus
        {
            Empty,
            InQueue,
            Dequeued
        };

        QueueStatus m_QueueStatus;

        std::string m_Title;
        std::string m_Subtitle;
        std::string m_TextureName;
        std::string m_SoundName;
        double m_WaitTime;

        ImageView* m_pImageView;
        TextView* m_pTitleTextView;
        TextView* m_pSubtitleTextView;
    };
}