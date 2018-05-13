#pragma once

#include "../VisualController.h"
#include <content/StaticMeshAllocator.h>
#include <logic/PfxManager.h>

namespace Logic
{
    class PfxVisual : public VisualController
    {
    public:
        /**
         * @param world World of the underlaying entity
         * @param entity Entity owning this controller
         */
        PfxVisual(World::WorldInstance& world, Handle::EntityHandle entity);
        virtual ~PfxVisual();

        /**
         * @return Classtype of this visual
         */
        EVisualType getVisualType() override { return EVisualType::Pfx; }
        /**
         * Called on game-tick
         */
        void onUpdate(float deltaTime) override;

        /**
         * Loads the visual from the given name from the worlds VDFS
         * Note: The type must match this class
         * @param visual Filename of the visual. Uncompiled-version of the name!
         */
        bool load(const std::string& visual) override;

        /**
         * @brief Called when something else modified the transform of the underlaying entity
         */
        void onTransformChanged() override;

    private:
        /**
         * Updates all render-object transforms
         */
        void updateRenderObjectTransforms();

        /**
         * Spawns a single particle after the rules of the emitter
         */
        void spawnParticle();

        /**
         * Updates a single particle
         */
        void updateParticle(Components::PfxComponent::Particle& p, float deltaTime);

        /**
         * Updates the bbox around the whole system to contain the given particle
         */
        void updateBBoxForParticle(Components::PfxComponent::Particle& p);

        /**
         * @return Reference to the underlaying PFX component
         */
        Components::PfxComponent& getPfxComponent();

        /**
         * Emitter to use
         */
        PfxManager::Emitter m_Emitter;

        /**
         * Keep track of the time since we last spawned a particle. So that high FPS numbers won't keep the "ppsValue" too low
         */
        float m_TimeSinceLastSpawn;

        /**
         * Current ppsScaleKey (with fraction to the next)
         */
        float m_ppsScaleKey;
        float m_shpScaleKey;

        /**
         * Current location on the spawning line or circle
         */
        float m_spawnPosition;

        /**
         * BBox around the particles
         */
        Utils::BBox3D m_BBox;

        /**
         * Renderhandle of the particlesystem (which is basically a dynamic vertex buffer)
         */
        Handle::RenderHandle m_MainRenderHandle;
    };
}
