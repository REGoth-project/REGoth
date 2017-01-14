//
// Created by desktop on 20.08.16.
//

#include <render/RenderSystem.h>
#include "PrintScreenMessages.h"
#include "View.h"
#include "zFont.h"
#include <engine/BaseEngine.h>

using namespace UI;

/**
 * How much time the simple printscreen-messages have before being popped
 */
const double TIME_TO_POP_MESSAGE = 1.0;

PrintScreenMessages::PrintScreenMessages(Engine::BaseEngine& e) : View(e)
{
    m_TimeToPopMessage = TIME_TO_POP_MESSAGE;
}

PrintScreenMessages::~PrintScreenMessages()
{

}

void PrintScreenMessages::update(double dt, Engine::MouseState& mstate, Render::RenderConfig& config)
{
    if(m_IsHidden)
        return;

    View::update(dt, mstate, config);

    const UI::zFont* fnt = m_Engine.getFontCache().getFont(DEFAULT_FONT);

    if(!fnt)
        return;

    // Do printscreen messages
    m_TimeToPopMessage -= dt;

    if(m_TimeToPopMessage < 0)
    {
        // Let older messages rise
        printMessage("");
        m_TimeToPopMessage = TIME_TO_POP_MESSAGE;
    }

    // Draw printscreen messages
    int y = -fnt->getFontHeight() * 3; // Start over the centered timed messages
    for(std::string& s : m_PrintMessages)
    {
        if(!s.empty())
            drawText(s,
                     config.state.viewWidth / 2,
                     (config.state.viewHeight / 2) + y,
                     A_Center,
                     config);

        y -= fnt->getFontHeight();
    }

    // Draw and update timed message
    drawTimedMessages(dt, config);
}

void PrintScreenMessages::printMessage(const std::string& message)
{
    if(m_PrintMessages.size() >= MAX_NUM_MESSAGES)
        m_PrintMessages.pop_back();

    m_PrintMessages.push_front(message);

    m_TimeToPopMessage = TIME_TO_POP_MESSAGE;
}

void PrintScreenMessages::drawTimedMessages(double dt, Render::RenderConfig& config)
{
    for(TimedMessage& msg : m_TimedMessages)
    {
        uint16_t width = static_cast<uint16_t>((config.state.viewWidth));
        uint16_t height = static_cast<uint16_t>((config.state.viewHeight));
        uint16_t posx, posy;

        // Center
        if(msg.x < 0)
            posx = static_cast<uint16_t>((width / 2));
        else
            posx = static_cast<uint16_t>(width * msg.x);

        if(msg.y < 0)
            posy = static_cast<uint16_t>(height / 2);
        else
            posy = static_cast<uint16_t>(height * msg.y);

        drawText(msg.message,
                 posx,
                 posy,
                 A_Center, // FIXME: This doesn't have to be centered all the time, right?
                 config);

        msg.timeLeft -= dt;
    }

    // Remove outtimed message
    for(auto it=m_TimedMessages.begin();it!=m_TimedMessages.end();)
    {
        if((*it).timeLeft < 0)
            it = m_TimedMessages.erase(it);
		else 
			it++;
    }
};

void PrintScreenMessages::printMessageTimed(float percx, float percy, const std::string& message, double time)
{
    TimedMessage msg;
    msg.x = percx;
    msg.y = percy;
    msg.message = message;
    msg.timeLeft = time;

    m_TimedMessages.push_back(msg);
}
