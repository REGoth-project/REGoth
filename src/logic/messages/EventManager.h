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
        void onMessage(const T& msg, Handle::EntityHandle sourceVob = Handle::EntityHandle::makeInvalidHandle())
        {
            // Copy over the data from the given message so only we handle the memory allocations
            T* msgcopy = new T;
            *msgcopy = msg;

            // Handle the message and potentially add it to the queue
            handleMessage(msgcopy, sourceVob);

            // Check if this was already executed. If so, we can delete it already. If not, it was added to the queue.
            if(msgcopy->deleted){
                delete msgcopy;
            }
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
         * @return Pointer to the last non-overlay conv-message, if found. If not, nullptr. DO NOT STORE THIS!
         */
        EventMessages::EventMessage* findLastConvMessageWith(Handle::EntityHandle other);

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
        void waitForMessage(EventMessages::EventMessage* other);

        /**
         * Cancels the current talking line of the npc
         */
        void cancelTalk();

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
        void handleMessage(EventMessages::EventMessage* message, Handle::EntityHandle sourceVob);

        /**
         * Sends the given message to the host-vob
         */
        void sendMessageToHost(EventMessages::EventMessage& message, Handle::EntityHandle sourceVob = Handle::EntityHandle::makeInvalidHandle());

        /**
         * Events registered and managed here.
         */
        std::vector<EventMessages::EventMessage*> m_EventQueue;

        /**
         * Vob this belongs to
         */
        Handle::EntityHandle m_HostVob;

        /**
         * World this is in
         */
        World::WorldInstance& m_World;

        /**
         * Creates a new ticket ID to identify the message when removing it on callback
         * @return new ticket ID
         */
        unsigned int drawTicket(){
            return m_ticketCounter++;
        }

        /**
         * ticket Counter for this EventManager instance
         */
        unsigned int m_ticketCounter;

        /**
         * finds the first ConversationMessage with the given ticket ID (if any) and marks it as deleted
         */
        void removeWaitingMessage(unsigned int ticket);
    };
}
