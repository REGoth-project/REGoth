//
// Created by markusobi on 19.05.17.
//

#include "LoadingScreen.h"
#include <engine/BaseEngine.h>
#include <render/RenderSystem.h>

UI::LoadingScreen::LoadingScreen(Engine::BaseEngine &e) : ImageView(e) {
    Handle::TextureHandle bgr = e.getEngineTextureAlloc().loadTextureVDF("LOADING.TGA");
    setImage(e.getEngineTextureAlloc().getTexture(bgr).m_TextureHandle);
}

UI::LoadingScreen::~LoadingScreen() {
}

void UI::LoadingScreen::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config) {
    m_ImageWidth = config.state.viewWidth;
    m_ImageHeight = config.state.viewHeight;
    ImageView::update(dt, mstate, config);
}
