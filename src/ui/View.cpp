//
// Created by desktop on 11.08.16.
//

#include <assert.h>
#include "View.h"

using namespace UI;

View::View()
{
    m_IsHidden = false;
}

View::~View()
{
    // Make sure all children have been removed
    assert(m_Children.empty());
}

void View::removeChild(View *pView)
{
    // Search for the view in our vector
    for(size_t i=0;i<m_Children.size();i++)
    {
        if(m_Children[i] == pView)
        {
            // Overwrite with last element and shorten the vector by 1
            m_Children[i] = m_Children.back();
            m_Children.pop_back();
            break;
        }
    }
}

void View::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    if(m_IsHidden)
        return;

    // Update all children
    for(View* v : m_Children)
    {
        v->update(dt, mstate, config);
    }
}
