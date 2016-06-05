#pragma once
#include "BaseEngine.h"
#include <render/RenderSystem.h>
#include <logic/CameraController.h>

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
        virtual void initEngine() override;

        /**
         * @brief Adds the main camera to the given world. If there already exists a main camera, it will be overwritten.
         */
        Handle::EntityHandle createMainCameraIn(Handle::WorldHandle world);

        /**
         * @return The given component of the main camera
         */
        template<typename T>
        T& getMainCamera()
        {
            return getMainWorld().getComponentAllocator().getElement<T>(m_MainCamera);
        }

        /**
         * @return Camera-Controller of the current main camera
         */
        Logic::CameraController* getMainCameraController()
        {
            return reinterpret_cast<Logic::CameraController*>(
                    getMainCamera<Components::LogicComponent>().m_pLogicController);
        }

        /**
         * @return data-access to the main world
         */
        World::WorldInstance& getMainWorld()
        {
            return m_WorldInstances.getElement(m_MainWorld);
        }
    protected:

        /**
		 * Called when a world was added
		 */
        virtual void onWorldCreated(Handle::WorldHandle world) override;

        /**
		 * Update-method for subclasses
		 */
        void onFrameUpdate(double dt) override;

        /**
         * Draws the worlds and presents the frame
         */
        void drawFrame();

        /**
         * Players camera entity
         */
        Handle::EntityHandle m_MainCamera;

        /**
         * Main world of this engine-instance
         */
        Handle::WorldHandle m_MainWorld;

        /**
         * Default rendering system
         */
        Render::RenderSystem m_DefaultRenderSystem;
    };
}