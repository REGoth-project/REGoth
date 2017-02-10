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
        bool print_queue = false;
        if (print_queue){
            std::vector<std::string> queue_strings;
            for(EventMessages::EventMessage* ev : m_EventQueue)
            {
                if (ev->messageType == EventMessages::EventMessageType::Conversation)
                {
                    EventMessages::ConversationMessage& conv_message = reinterpret_cast<EventMessages::ConversationMessage&>(*ev);
                    std::string debug_out;
                    if (conv_message.subType == EventMessages::ConversationMessage::ST_WaitTillEnd){
                        auto waiting_for = reinterpret_cast<const EventMessages::ConversationMessage*>(conv_message.waitIdentifier);
                        queue_strings.push_back("ST_WaitTillEnd, ident = " + waiting_for->text);
                    } else if (conv_message.subType == EventMessages::ConversationMessage::ST_Output){
                        queue_strings.push_back(conv_message.text);
                   }
                }
            }
            if (queue_strings.size() != 0){
                LogInfo() << "----------------------Queue-Start----------------------";
                for (auto& item : queue_strings)
                   LogInfo() << item;
                LogInfo() << "----------------------Queue-End------------------------";
            }
        }
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

    bool print_queue = false;
    if (print_queue){
        bool dialog_ready_to_play = false;
        for(EventMessages::EventMessage* ev : m_EventQueue)
        {
            if (ev->messageType == EventMessages::EventMessageType::Conversation)
            {
                EventMessages::ConversationMessage* conv_message = reinterpret_cast<EventMessages::ConversationMessage*>(ev);
                if (!conv_message->internInProgress && conv_message->text != std::string(""))
                {
                    dialog_ready_to_play = true;
                    break;
                }
            }
            if(!ev->isOverlay)
                break;
        }
        if (dialog_ready_to_play){
            LogInfo() << "----------------------Queue-Start----------------------";
            for(EventMessages::EventMessage* ev : m_EventQueue)
            {
                if (ev->messageType == EventMessages::EventMessageType::Conversation)
                {
                    EventMessages::ConversationMessage* conv_message = reinterpret_cast<EventMessages::ConversationMessage*>(ev);
                    if (!conv_message->internInProgress && conv_message->text != std::string(""))
                    {
                        LogInfo() << "Queue item: " << std::boolalpha << "block = " << !ev->isOverlay << ", text = " << conv_message->text;
                    } else if (conv_message->subType == EventMessages::ConversationMessage::ST_WaitTillEnd){
                        auto waiting_for = reinterpret_cast<const EventMessages::ConversationMessage*>(conv_message->waitIdentifier);
                        LogInfo() << "Queue item: " << std::boolalpha << "block = " << !ev->isOverlay << ", waiting for: " << conv_message << ", ident = " << waiting_for->text;
                    }
                }
            }
            LogInfo() << "----------------------Queue-End----------------------";
        }
    }

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
            auto conv = reinterpret_cast<EventMessages::ConversationMessage*>(ev);
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

    // Let the EM wait for this talking-action to complete
    EventMessages::ConversationMessage* waitMessage = onMessage(wait);

    other->onMessageDone.push_back(std::make_pair(m_HostVob, [=](Handle::EntityHandle hostVob, EventMessages::EventMessage* inst) {
        // case: wait message not there anymore, i.e. player/NPC got killed while conversation
        if (std::find(this->m_EventQueue.begin(), this->m_EventQueue.end(), waitMessage) == this->m_EventQueue.end()){
            return;
        }
        // case: wait not queued, message handled instantly
        if (waitMessage == nullptr)
            return;

        // Mark as done
        waitMessage->deleted = true;

        const EventMessages::ConversationMessage* waiting_for = reinterpret_cast<const EventMessages::ConversationMessage*>(waitMessage->waitIdentifier);
        LogInfo() << "WAIT CALLBACK message completed: ptr = " << waitMessage << ", text = " << waiting_for->text;
        //triggerWaitEvent(inst, waitMessage);
    }));

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
