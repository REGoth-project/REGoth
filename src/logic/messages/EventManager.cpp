//
// Created by desktop on 16.08.16.
//

#include <components/Vob.h>
#include <components/VobClasses.h>
#include <ZenLib/utils/logger.h>
#include "EventManager.h"
#include <json.hpp>

using json = nlohmann::json;
using namespace Logic;

EventManager::EventManager(World::WorldInstance& world, Handle::EntityHandle hostVob) :
    m_World(world),
    m_HostVob(hostVob),
    m_ticketCounter(1)
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
        /*if(message->messageType == EventMessages::EventMessageType::Conversation)
        {
            EventMessages::ConversationMessage* conv = (EventMessages::ConversationMessage*)message;

            if(conv->subType == EventMessages::ConversationMessage::ST_PlayAni)
            {
                LogInfo() << "Set state from: " << m_World.getScriptEngine().getVM().getCallStack();
            }
        }*/

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
    // Not using iterators here, because a message might get pushed inside a callback, which
    // would make them invalid. This has to be done before deleting the message for this very reason.
    for(size_t i=0, end=m_EventQueue.size();i<end;i++)
    {
        if(m_EventQueue[i]->deleted)
        {
            // Trigger done-callbacks
            for(auto cb : m_EventQueue[i]->onMessageDone)
            {
                cb.second(cb.first, m_EventQueue[i]);
            }
        }
    }

    // Remove deleted messages from last time
    for(auto it = m_EventQueue.begin();it != m_EventQueue.end();)
    {
        if((*it)->deleted)
        {
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

EventMessages::EventMessage* EventManager::findLastConvMessageWith(Handle::EntityHandle other)
{
    auto begin = m_EventQueue.rbegin();
    auto end = m_EventQueue.rend();
    auto lastConvMessage = std::find_if(begin, end, [&](EventMessages::EventMessage* ev){
        if(!ev->isOverlay && ev->messageType == EventMessages::EventMessageType::Conversation)
        {
            auto conv = dynamic_cast<EventMessages::ConversationMessage*>(ev);
            return conv->target == other;
        }
        return false;
    });
    if (lastConvMessage == end){
        return nullptr;
    } else {
        return *lastConvMessage;
    }
}

bool EventManager::hasConvMessageWith(Handle::EntityHandle other){
    return findLastConvMessageWith(other) != nullptr;
}

void EventManager::waitForMessage(EventMessages::EventMessage* other)
{
    // Push a wait-message first
    EventMessages::ConversationMessage wait;
    wait.subType = EventMessages::ConversationMessage::ST_WaitTillEnd;
    wait.waitIdentifier = other;
    unsigned int ticket = drawTicket();
    wait.waitTicket = ticket;

    // Let the EM wait for this talking-action to complete
    onMessage(wait);

    other->onMessageDone.push_back(std::make_pair(m_HostVob, [=](Handle::EntityHandle hostVob, EventMessages::EventMessage* inst) {
        // possible FIXME: handle case where EventManager was deleted.
        removeWaitingMessage(ticket);
    }));
}

void EventManager::removeWaitingMessage(unsigned int ticket){
    for (EventMessages::EventMessage* message : m_EventQueue){
        if (message->messageType == EventMessages::EventMessageType::Conversation){
            auto conv_message = dynamic_cast<EventMessages::ConversationMessage*>(message);
            if (conv_message->waitTicket == ticket){
                // Mark as done
                conv_message->deleted = true;
                break;
            }
        }
    }
}

void EventManager::cancelTalk(){
    for (EventMessages::EventMessage* message : m_EventQueue){
        if (message->messageType == EventMessages::EventMessageType::Conversation){
            auto conv_message = dynamic_cast<EventMessages::ConversationMessage*>(message);
            if (conv_message->subType == EventMessages::ConversationMessage::ST_Output && conv_message->internInProgress){
                // Mark as done
                conv_message->deleted = true;
                return;
            }
        }
    }
}

void EventManager::clear()
{
    for(EventMessages::EventMessage* ev : m_EventQueue)
    {
        ev->deleted = true;
    }
}

bool EventManager::isEmpty()
{
    for(EventMessages::EventMessage* ev : m_EventQueue)
    {
        if(!ev->deleted)
            return false;
    }

    return true;
}

std::string EventManager::exportObject()
{
    return "";
}
