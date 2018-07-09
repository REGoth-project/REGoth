//
// Created by markusobi on 04.06.18.
//

#pragma once
#include "View.h"

namespace UI
{
class WorldView : public View
{
public:
    WorldView(Engine::BaseEngine& e);

    /**
     * Draws the World
     */
    void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config) override;
};
}