#pragma once

#include <handle/HandleDef.h>
#include <daedalus/DaedalusGameState.h>
#include <logic/visuals/ModelVisual.h>
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
     * Sets the model heirachy from the given visual (MDS)
     * @param vob npc to operate on
     * @param visual heirachy to set
     */
    void NPC_SetModelVisual(NpcVobInformation& vob, const std::string& visual);

    /**
     * Sets the head of the given NPC
     * @param vob NPC to operate on
     * @param visual Visual to load and set
     * @param headTextureIdx Index of the texture to use for the head
     * @param teethTextureIdx Index of the texture to use for the teeth
     */
    void NPC_SetHeadMesh(NpcVobInformation& vob, const std::string& visual, size_t headTextureIdx=0, size_t teethTextureIdx=0);

    /**
     * Sets the visual on the given NPCs model, without changing the body-state, like the headmesh
     * @param vob NPC to operate on
     * @param visual Visual to load and set
     */
    void NPC_ReplaceMainVisual(NpcVobInformation& vob, const std::string& visual);

    /**
     * Equips the given weapon to the NPC
     * @param vob NPC to operate on
     * @param weapon Script-Handle to the weapon
     */
    void NPC_EquipWeapon(NpcVobInformation& vob, Daedalus::GameState::ItemHandle weapon);

    /**
     * @return The engine entity handle of the given script instance of an npc
     */
    Handle::EntityHandle getEntityFromScriptInstance(World::WorldInstance& world, Daedalus::GameState::NpcHandle npc);
}