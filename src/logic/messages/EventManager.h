#pragma once

#include <handle/HandleDef.h>
#include <list>
#include "EventMessage.h"

namespace World
{
    class WorldInstance;
}

namespace Logic
{
    class EventManager
    {
    public:
        EventManager(World::WorldInstance& world, Handle::EntityHandle hostVob);
        ~EventManager();

        template<typename T>
        void onMessage(T& msg, Handle::EntityHandle sourceVob = Handle::EntityHandle::makeInvalidHandle())
        {
            // Copy over the data from the given message so only we handle the memory allocations
            T* msgcopy = new T;
            *msgcopy = msg;

            // Handle the message and potentially add it to the queue
            handleMessage(msgcopy, sourceVob);

            // Check if this was already executed. If so, we can delete it already. If not, it was added to the queue.
            if(msgcopy->deleted)
                delete msgcopy;
        }

        /**
         * Processes the messagequeue.
         * This might lead to the destruction of the host-vob and this object in response to a message!
         */
        void processMessageQueue();

        /**
         * @return
         */
        bool isEmpty(){ return m_EventQueue.empty(); }

    protected:

        /**
         * Handles an incomming message. This will decide whether or not to immediately execute the message or
         * whether it should wait in the queue. Immediate messages will be flagged as to-delete after execution
         * @param message Message to handle
         */
        void handleMessage(EventMessages::EventMessage* message, Handle::EntityHandle sourceVob);

        /**
         * Sends the given message to the host-vob
         */
        void sendMessageToHost(EventMessages::EventMessage& message, Handle::EntityHandle sourceVob = Handle::EntityHandle::makeInvalidHandle());

        /**
         * Events registered and managed here.
         */
        std::list<EventMessages::EventMessage*> m_EventQueue;

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