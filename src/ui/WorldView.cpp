//
// Created by markusobi on 04.06.18.
//

#include "WorldView.h"

namespace UI
{
    WorldView::WorldView(Engine::BaseEngine& e)
        : View(e)
    {
    }

    void WorldView::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
    {
        //cleanMenus(); // TODO: think about when this should be called
    }
}