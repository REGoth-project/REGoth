#pragma once
#include <common.h>
#include <vector>
#include <render/RenderSystem.h>
#include <engine/Input.h>

namespace UI
{
    /**
     * Base UI-View
     */
    class View
    {
    public:
        View();

        virtual ~View();

        /**
         * Adds a subview to this view.
         * Note: Pointer only stored for usage, freeing the memory is up to the caller.
         *       All children have to be removed from this, before this can be deleted, to ensure no memory leaks.
         * @param view View to add
         */
        void addChild(View *pView)
        { m_Children.push_back(pView); }

        /**
         * Removes a subview from this view
         * @param pView View to remove
         */
        void removeChild(View *pView);

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        virtual void update(double dt, Engine::Input::MouseState &mstate, Render::RenderConfig &config);

        /**
         * Set/get hidden
         */
        bool isHidden(){ return m_IsHidden; }
        void setHidden(bool v){ m_IsHidden = v; }
    protected:

        /**
         * Sub-views drawn on top of this one
         */
        std::vector<View *> m_Children;

        /**
         * Whether this should be drawn/processed
         */
        bool m_IsHidden;
    };
}