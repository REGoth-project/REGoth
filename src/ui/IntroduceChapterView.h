
#pragma once

#include "View.h"
#include "ImageView.h"
#include "TextView.h"
#include <handle/HandleDef.h>

namespace UI
{
    class IntroduceChapterView : public View
    {
    public:
        IntroduceChapterView(Engine::BaseEngine& e);

        ~IntroduceChapterView() override;

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config) override;

        /**
         * Queues up the ChapterIntroduction request
         * @param title Title to display at the top of the view
         * @param subtitle Subtitle to dispaly at the bottom of the view
         * @param texture_name Name of the image to display
         * @param sound_name Name of the sound to play
         ' @param wait_time Duration to display this view for
         */
        void enqueueChapterIntroduction(std::string title, std::string subtitle, std::string texture_name, std::string sound_name, double wait_time);

        /**
         * Closes this view by resetting it
         */
        void close();
        
    private:
        /**
         * Enum to show status of the requested ChapterIntroduction in the Queue
         * Empty stops this view from being updated
         * InQueue sets up Image, Text and Sound; plays Sound and pauses Engine
         * Dequeued makes this view update as expected
         */
        enum QueueStatus
        {
            Empty,
            InQueue,
            Dequeued
        };
        /**
         * Status of the requested ChapterIntroduction
         */
        QueueStatus m_QueueStatus;

        /**
         * Title to the top of the view and Subtitle to the bottom of the view
         */
        std::string m_Title;
        std::string m_Subtitle;

        /**
         * Image to display
         */
        Handle::TextureHandle m_Texture;

        /**
         * Sound to play
         */
        Handle::SfxHandle m_Sound;

        /**
         * Duration to display the view for
         */
        double m_WaitTime;

        /**
         * ImageView for the Image to display
         * TextView for the Title to display
         * TextView for the Subtitle to display
         */
        ImageView* m_pImageView;
        TextView* m_pTitleTextView;
        TextView* m_pSubtitleTextView;
    };
}