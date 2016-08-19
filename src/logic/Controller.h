#pragma once
#include <handle/HandleDef.h>
#include <components/Entities.h>
#include "messages/EventManager.h"

namespace World
{
    class WorldInstance;
}

namespace Logic
{
    namespace EventMessages
    {
        class EventMessage;
    }

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
        virtual void onUpdate(float deltaTime);

        /**
         * Called at rendertime
         */
        virtual void onDebugDraw(){}

        /**
         * Called when this vob recieved a message
         */
        virtual void onMessage(EventMessages::EventMessage& message, Handle::EntityHandle sourceVob = Handle::EntityHandle::makeInvalidHandle()){}

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

        /**
         * @return Event manager for this logic-controller
         */
        EventManager& getEM(){ return m_EventManager; }
    protected:
        /**
         * Entity owning this controller
         */
        Handle::EntityHandle m_Entity;

        /**
         * Allocator the underlaying entity was created with
         */
        World::WorldInstance& m_World;

        /**
         * Event manager for this logic-controller
         */
        EventManager m_EventManager;
    };
}