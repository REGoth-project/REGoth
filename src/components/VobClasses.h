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
     * Helper-function to insert an NPC into the world (With script initialization)
     * Same as calling Wld_InsertNPC from script!
     * @param world World to add the npc to
     * @param instanceName Script-instance to create
     * @param wpName Waypoint to put the npc on
     * @return Handle to the NPCs entity
     */
    Handle::EntityHandle Wld_InsertNpc(World::WorldInstance& world, const std::string& instanceName, const std::string& wpName = "");

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
    void NPC_SetBodyMesh(NpcVobInformation &vob, const std::string &visual, int bodyTexIdx=-1, int skinColorIdx=-1);

    /**
     * Equips the given weapon to the NPC
     * @param vob NPC to operate on
     * @param weapon Script-Handle to the weapon
     */
    void NPC_EquipWeapon(NpcVobInformation& vob, Daedalus::GameState::ItemHandle weapon);

    /**
     * Draws the currently equipped melee weapon
     * @param vob NPC which should draw the weapon
     * @return Weapon that was drawn
     */
    Daedalus::GameState::ItemHandle NPC_DrawMeleeWeapon(NpcVobInformation& npc);

    /**
     * Puts back any weapon a NPC has in its hand
     * @param npc NPC to perform the action on
     */
    void NPC_UndrawWeapon(NpcVobInformation& npc);

    /**
     * Returns the script-parameters for the given npc
     * @param vob Npc to get the info from
     * @return Script-side object of this npc
     */
    Daedalus::GEngineClasses::C_Npc& getScriptObject(NpcVobInformation& vob);

	/**
	 * Returns the script handle for the given npc 
	 * @param vob Npc to get the info from
	 * @return Script-side handle of this npc
	 */
	Daedalus::GameState::NpcHandle getScriptHandle(VobTypes::NpcVobInformation& vob);

    /**
     * @return The engine entity handle of the given script instance of an npc
     */
    Handle::EntityHandle getEntityFromScriptInstance(World::WorldInstance& world, Daedalus::GameState::NpcHandle npc);
    NpcVobInformation getVobFromScriptHandle(World::WorldInstance& world, Daedalus::GameState::NpcHandle npc);
}