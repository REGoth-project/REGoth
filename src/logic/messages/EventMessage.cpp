//
// Created by desktop on 16.08.16.
//

#include "EventMessage.h"
#include <json.hpp>

using json = nlohmann::json;

std::string Logic::EventMessages::MobMessage::exportPart()
{
    return "";
}

std::string Logic::EventMessages::MagicMessage::exportPart()
{
    return NpcMessage::exportPart();
}

std::string Logic::EventMessages::ConversationMessage::exportPart()
{
    return NpcMessage::exportPart();
}

std::string Logic::EventMessages::ManipulateMessage::exportPart()
{
    return NpcMessage::exportPart();
}

std::string Logic::EventMessages::StateMessage::exportPart()
{
    return NpcMessage::exportPart();
}

std::string Logic::EventMessages::UseItemMessage::exportPart()
{
    return NpcMessage::exportPart();
}

std::string Logic::EventMessages::AttackMessage::exportPart()
{
    return NpcMessage::exportPart();
}

std::string Logic::EventMessages::MovementMessage::exportPart()
{
    return NpcMessage::exportPart();
}

std::string Logic::EventMessages::WeaponMessage::exportPart()
{
    return NpcMessage::exportPart();
}

std::string Logic::EventMessages::DamageMessage::exportPart()
{
    return NpcMessage::exportPart();
}

std::string Logic::EventMessages::NpcMessage::exportPart()
{
    return "";
}

std::string Logic::EventMessages::EventMessage::exportPart()
{
    return "";
}
