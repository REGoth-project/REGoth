#pragma once

#include "View.h"
#include <handle/HandleDef.h>

namespace UI
{
    class Console;
    class ConsoleBox : public View
    {
    public:
        ConsoleBox(Engine::GameEngine& e);
        ~ConsoleBox();

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config) override;

        /**
         * Increases the selection index by the given amount.
         * performs periodic wrap around.
         * @param amount may be positive or negative
         */
        void increaseSelectionIndex(int amount);

        /**
         * sets the current selection index
         * clamps index in range [-1; size-1]
         */
        void setSelectionIndex(int newIndex);

        int getSelectionIndex() const { return m_CurrentlySelected; }
    protected:
        /**
         * Console background image
         */
        Handle::TextureHandle m_BackgroundTexture;

        /**
         * Suggestions background image
         */
        Handle::TextureHandle m_SuggestionsBackgroundTexture;

        /**
         * Index of selected entry
         */
        int m_CurrentlySelected;
    };
}
