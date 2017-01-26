#pragma once

#include <content/StaticMeshAllocator.h>
#include "../VisualController.h"

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
        virtual EVisualType getVisualType(){ return EVisualType::Pfx; }

        /**
         * Called on game-tick
         */
        void onUpdate(float deltaTime) override;

        /**
         * Loads the visual from the given name from the worlds VDFS
         * Note: The type must match this class
         * @param visual Filename of the visual. Uncompiled-version of the name!
         */
        virtual bool load(const std::string& visual);
    private:

        /**
         * Single particle
         */
        struct Particle
        {
            Math::float3 position;
            Math::float3 velocity;
            float lifetime;
            float texAniFrame;
            float alphaVel;
            float alpha;
            Math::float2 size;
            Math::float2 sizeVel;
            Math::float3 color;
            Math::float3 colorVel;
        };

        /**
         * Spawns a single particle after the rules of the emitter
         */
        void spawnParticle();

        /**
         * Updates a single particle
         */
        void updateParticle(Particle& p, float deltaTime);

        /**
         * @return Reference to the underlaying PFX component
         */
        Components::PfxComponent& getPfxComponent();

        /**
         * Particle cache
         */
        std::vector<Particle> m_Particles;
        std::vector<Meshes::WorldStaticMeshVertex> m_QuadVertices; // m_Particles.size() * 6

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
    };
}

