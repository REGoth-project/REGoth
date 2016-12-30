#pragma once
#include <json.hpp> // I am not sure about this. Pretty big header inside a header that is inclueded often.
                    // But sadly you cant easily forward-declare nlohmann::json...
using json = nlohmann::json;

#include <handle/HandleDef.h>
#include <components/Entities.h>
#include "messages/EventManager.h"
#include "ControllerTypes.h"



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
         * @return The type of this class. If you are adding a new base controller, be sure to add it to ControllerTypes.h
         */
        virtual EControllerType getControllerType(){ return EControllerType::Controller; };

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

        /**
         * Exports this object as json-string. Includes the whole dependency-tree.
         */
        void exportObject(json& j);
        virtual void importObject(const json& j);
        //void importPart(const std::string& part);

        /**
         * @return Classes which want to get exported on save should return true here
         */
        virtual bool shouldExport(){ return false; }

    protected:

        /**
         * Exports this class only, ignores the base-classes values, but must call the function *BEFORE* writing values
         */
        virtual void exportPart(json& j);

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