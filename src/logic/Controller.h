#pragma once
#include <handle/HandleDef.h>
#include <components/Entities.h>

namespace World
{
    class WorldInstance;
}

namespace Logic
{
    class Controller
    {
    public:
        /**
         * @param world World of the underlaying entity
         * @param entity Entity owning this controller
         */
        Controller(World::WorldInstance& world, Handle::EntityHandle entity);
        virtual ~Controller(){};

        /**
         * Called when the models visual changed
         */
        virtual void onVisualChanged(){};

        /**
         * Called on game-tick
         */
        virtual void onUpdate(float deltaTime){};

        /**
         * Called at rendertime
         */
        virtual void onDebugDraw(){}

        /**
         * Sets the transform of the underlaying entity
         */
        void setEntityTransform(const Math::Matrix& transform);

        /**
         * @return The current transform of the underlaying entity
         */
        Math::Matrix& getEntityTransform();

        /**
         * @brief Called when something else modified the transform of the underlaying entity
         */
        virtual void onTransformChanged(){};
    protected:
        /**
         * Entity owning this controller
         */
        Handle::EntityHandle m_Entity;

        /**
         * Allocator the underlaying entity was created with
         */
        World::WorldInstance& m_World;
    };
}