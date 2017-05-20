//
// Created by markusobi on 19.05.17.
//
#pragma once
#include "ImageView.h"

namespace UI
{
    class LoadingScreen : public ImageView
    {
    public:
        LoadingScreen(Engine::BaseEngine& e);
        ~LoadingScreen();

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config) override;

    protected:
    };
}

