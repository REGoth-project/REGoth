//
// Created by desktop on 16.08.16.
//

#include <components/Vob.h>
#include "EventManager.h"

using namespace Logic;

EventManager::EventManager(World::WorldInstance& world, Handle::EntityHandle hostVob) :
    m_World(world),
    m_HostVob(hostVob)
{

}

EventManager::~EventManager()
{
    // Free memory of all staging messages
    for(EventMessages::EventMessage* ev : m_EventQueue)
    {
        delete ev;
    }
    m_EventQueue.clear();
}

void EventManager::handleMessage(Logic::EventMessages::EventMessage* message, Handle::EntityHandle sourceVob)
{
    // Check if we shall execute this right away
    if(!message->isJob && (message->isHighPriority || m_EventQueue.empty()))
    {
        // Pass the message to the host
        sendMessageToHost(*message);

        // Flag as done
        message->deleted = true;
    }else
    {
        // Queue this
        m_EventQueue.push_back(message);
    }
}

void EventManager::sendMessageToHost(EventMessages::EventMessage& message, Handle::EntityHandle sourceVob)
{
    Vob::VobInformation vob = Vob::asVob(m_World, m_HostVob);

    vob.logic->onMessage(message, sourceVob);
}

void EventManager::processMessageQueue()
{
    // Remove deleted messages from last time
    for(auto it = m_EventQueue.begin();it != m_EventQueue.end(); it++)
    {
        if((*it)->deleted)
        {
            delete (*it);
            it = m_EventQueue.erase(it);
        }
    }

    if(m_EventQueue.empty())
        return;

    // Process messages as far as we can
    for(EventMessages::EventMessage* ev : m_EventQueue)
    {
        sendMessageToHost(*ev);

        // FIXME: This event manager could have been deleted as a reaction to the message! Take care of that!

        // Mark as done if this wasn't a job
        if(!ev->isJob)
            ev->deleted = true;

        if(!ev->isOverlay)
            break;
    }
}
