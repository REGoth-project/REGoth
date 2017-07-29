#pragma once

#include <handle/HandleDef.h>
#include <list>
#include <memory>
#include "EventMessage.h"

namespace World
{
    class WorldInstance;
}

namespace Logic
{
    class EventManager
    {
        using SharedEMessage = std::shared_ptr<Logic::EventMessages::EventMessage>;

    public:
        EventManager(World::WorldInstance& world, Handle::EntityHandle hostVob);

        template <typename T>
        std::shared_ptr<T> onMessage(const T& msg, Handle::EntityHandle sourceVob = Handle::EntityHandle::makeInvalidHandle())
        {
            // Copy over the data from the given message
            std::shared_ptr<T> copyDerived = std::make_shared<T>(msg);

            // Handle the message and potentially add it to the queue
            handleMessage(copyDerived, sourceVob);
            return copyDerived;
        }

        /**
         * Processes the messagequeue.
         * This might lead to the destruction of the host-vob and this object in response to a message!
         */
        void processMessageQueue();

        /**
         * @return
         */
        bool isEmpty();

        /**
         * Searches the messages for the last one that is a conversation message and has the NPC with the entity "other" as target
         * @param other NPC-Entity to search for
         * @return Pointer to the last non-overlay conv-message, if found. If not, nullptr.
         */
        SharedEMessage findLastConvMessageWith(Handle::EntityHandle other);

        /**
         * Searches the messages if any conversation message exists, that has the NPC with the entity "other" as target
         * @param other NPC-Entity to search for
         * @return whether message was found
         */
        bool hasConvMessageWith(Handle::EntityHandle other);

        /**
         * Blocks the event-queue until the given event was processed. Can be from another NPC!
         * @param other Message to wait for
         */
        void waitForMessage(SharedEMessage other);

        /**
         * Removes all pending messages
         */
        void clear();

        /**
         * Exports this as JSON-String
         * @return
         */
        std::string exportObject();

    protected:
        /**
         * Handles an incomming message. This will decide whether or not to immediately execute the message or
         * whether it should wait in the queue. Immediate messages will be flagged as to-delete after execution
         * @param message Message to handle
         */
        void handleMessage(SharedEMessage message, Handle::EntityHandle sourceVob);

        /**
         * Sends the given message to the host-vob
         */
        void sendMessageToHost(SharedEMessage message, Handle::EntityHandle sourceVob = Handle::EntityHandle::makeInvalidHandle());

        /**
         * Events registered and managed here.
         */
        std::vector<SharedEMessage> m_EventQueue;

        /**
         * Vob this belongs to
         */
        Handle::EntityHandle m_HostVob;

        /**
         * World this is in
         */
        World::WorldInstance& m_World;
    };
}
