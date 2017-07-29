#pragma once

#include <list>
#include "View.h"
#include <common.h>
#include <render/RenderSystem.h>

namespace UI
{
    class PrintScreenMessages : public View
    {
    public:
        PrintScreenMessages(Engine::BaseEngine& e);
        ~PrintScreenMessages();

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config) override;

        /**
         * Prints a message to the screen
         */
        void printMessage(const std::string& message);

        /**
         * Prints a timed message at the given position
         * @param percx Percentage (0..1) of the x axis
         * @param percy Percentage (0..1) of the y axis
         * @param message Message to display
         * @param time Time to stay in seconds
         */
        void printMessageTimed(float percx, float percy, const std::string& message, double time);

    protected:
        /**
         * Draws all timed messages
         */
        void drawTimedMessages(double dt, Render::RenderConfig& config);

        /**
         * Messages displayed on-screen for some time
         */
        enum
        {
            MAX_NUM_MESSAGES = 5
        };
        std::list<std::string> m_PrintMessages;
        double m_TimeToPopMessage;

        /**
         * List of currently active timed messages
         */
        struct TimedMessage
        {
            double timeLeft;
            float x, y;
            std::string message;
        };
        std::list<TimedMessage> m_TimedMessages;
    };
}