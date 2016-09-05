//
// Created by desktop on 16.08.16.
//

#include <components/Vob.h>
#include <components/VobClasses.h>
#include "EventManager.h"

using namespace Logic;

EventManager::EventManager(World::WorldInstance& world, Handle::EntityHandle hostVob) :
    m_World(world),
    m_HostVob(hostVob)
{

}

EventManager::~EventManager()
{
    // Remove all waiting callbacks we currently have out there
    for(EventMessages::EventMessage* ev : m_WaitingFor)
    {
        for(auto it=ev->onMessageDone.begin(); it != ev->onMessageDone.end(); it++)
        {
            if((*it).first == m_HostVob)
                it = ev->onMessageDone.erase(it);
        }
    }

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
    for(auto it = m_EventQueue.begin();it != m_EventQueue.end();)
    {
        if((*it)->deleted)
        {
            // Trigger done-callbacks
            for(auto cb : (*it)->onMessageDone)
            {
                cb.second((*it));
            }

            delete (*it);
            it = m_EventQueue.erase(it);
		}
		else
		{
			it++;
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

EventMessages::EventMessage* EventManager::getTalkingWithMessage(Handle::EntityHandle other)
{
    EventMessages::EventMessage* lastNonOverlay = nullptr;

    for(EventMessages::EventMessage* ev : m_EventQueue)
    {
        if(!ev->isOverlay)
        {
            lastNonOverlay = ev;
        }

        if(ev->messageType == EventMessages::EventMessageType::Conversation)
        {
            auto conv = reinterpret_cast<EventMessages::ConversationMessage*>(ev);
            if(conv->target == other)
                return lastNonOverlay;
        }
    }

    return nullptr;
}

void EventManager::triggerWaitEvent(EventMessages::EventMessage::MessageIdentifier identifier)
{
    // Trigger all messages we have with that identifier
    for(EventMessages::EventMessage* ev : m_EventQueue)
    {
        if(ev->messageType == EventMessages::EventMessageType::Conversation
                && ev->subType == EventMessages::ConversationMessage::ST_WaitTillEnd)
        {
            EventMessages::ConversationMessage* conv = reinterpret_cast<EventMessages::ConversationMessage*>(ev);

            if(conv->waitIdentifier == identifier)
            {
                // Mark as done
                conv->deleted = true;
            }
        }
    }

    // Remove waiting-handle
    // FIXME: This is some really ugly code, but I just want to remove the list-entry
    const EventMessages::EventMessage* msg = reinterpret_cast<const EventMessages::EventMessage*>(identifier);
    m_WaitingFor.remove(const_cast<EventMessages::EventMessage*>(msg));
}

void EventManager::waitForMessage(EventMessages::EventMessage* other)
{
    // Push a wait-message first
    EventMessages::ConversationMessage wait;
    wait.subType = EventMessages::ConversationMessage::ST_WaitTillEnd;
    wait.waitIdentifier = other;

    // Let the EM wait for this talking-action to complete
    onMessage(wait);

    other->onMessageDone.push_back(std::make_pair(m_HostVob, [=](EventMessages::EventMessage* inst) {

        // Let the other NPC know we are done
        triggerWaitEvent(inst);
    }));

}
