#pragma once

#include <handle/HandleDef.h>
#include <daedalus/DaedalusGameState.h>
#include "Vob.h"

namespace VobTypes
{
    struct ScriptInstanceUserData
    {
        // In case we want subtypes?
        virtual ~ScriptInstanceUserData(){}

        /**
         * Engine-Side entity of this script instance
         */
        Handle::EntityHandle vobEntity;
    };

    /**
     * Creates a generic vob from script
     */
    Handle::EntityHandle initNPCFromScript(World::WorldInstance& world, Daedalus::GameState::NpcHandle scriptInstance);

    /**
     * Unlinks the script-instance from the engine. If this is not done, it will result in a memory-leak.
     */
    void unlinkNPCFromScriptInstance(World::WorldInstance& world, Handle::EntityHandle entity, Daedalus::GameState::NpcHandle scriptInstance);
}