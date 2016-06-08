#pragma once

#include <handle/HandleDef.h>
#include <daedalus/DaedalusGameState.h>
#include "Vob.h"

namespace Logic
{
    class PlayerController;
}

namespace VobTypes
{
    struct ScriptInstanceUserData
    {
        /**
         * Engine-Side entity of this script instance
         */
        Handle::EntityHandle vobEntity;
        Handle::WorldHandle world;
    };

    struct NpcVobInformation : Vob::VobInformation
    {
        Logic::PlayerController* playerController;
    };

    /**
     * Returns an Entity as NPC-Vob
     */
    /**
     * Extracts the vob-information from the given entity
     * NOTE: ONLY FOR TEMPORARY USE. DO NOT SAVE THE RETURNED OBJECT FOR LATER USE.
     */
    NpcVobInformation asNpcVob(World::WorldInstance& world, Handle::EntityHandle e);

    /**
     * Creates a generic vob from script
     */
    Handle::EntityHandle initNPCFromScript(World::WorldInstance& world, Daedalus::GameState::NpcHandle scriptInstance);

    /**
     * Unlinks the script-instance from the engine. If this is not done, it will result in a memory-leak.
     */
    void unlinkNPCFromScriptInstance(World::WorldInstance& world, Handle::EntityHandle entity, Daedalus::GameState::NpcHandle scriptInstance);

    /**
     * @return The engine entity handle of the given script instance of an npc
     */
    Handle::EntityHandle getEntityFromScriptInstance(World::WorldInstance& world, Daedalus::GameState::NpcHandle npc);
}