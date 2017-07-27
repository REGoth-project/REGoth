#pragma once
#include "BaseEngine.h"
#include <render/RenderSystem.h>
#include <logic/CameraController.h>
#include "Input.h"

namespace Engine
{
    class GameEngine : public BaseEngine
    {
    public:
        GameEngine();
        ~GameEngine();

        /**
		 * @brief Initializes the Engine
		 */
        virtual void initEngine(int argc, char** argv) override;

        /**
         * @return Default set of shaders and constants
         */
        Render::RenderSystem& getDefaultRenderSystem()
        {
            return m_DefaultRenderSystem;
        }

    protected:
        /**
		 * Called when a world was added
		 */
        virtual void onWorldCreated(Handle::WorldHandle world) override;
        virtual void onWorldRemoved(Handle::WorldHandle world) override;

        /**
		 * Update-method for subclasses
		 */
        void onFrameUpdate(double dt, uint16_t width, uint16_t height) override;

        /**
         * Draws the worlds and presents the frame
         */
        void drawFrame(uint16_t width, uint16_t height);

        /**
         * Default rendering system
         */
        Render::RenderSystem m_DefaultRenderSystem;
    };
}
