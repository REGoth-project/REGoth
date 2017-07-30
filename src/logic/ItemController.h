#pragma once
#include <set>
#include "Controller.h"
#include "LogicDef.h"
#include <daedalus/DaedalusGameState.h>

namespace Logic
{
    class ItemController : public Controller
    {
    public:
        /**
		 * @param world World of the underlaying entity
		 * @param entity Entity owning this controller
		 */
        ItemController(World::WorldInstance& world, Handle::EntityHandle entity, size_t scriptInstance);

        /**
         * @return The type of this class. If you are adding a new base controller, be sure to add it to ControllerTypes.h
         */
        virtual EControllerType getControllerType() { return EControllerType::ItemController; }
        /**
		 * Removes this item from the world and adds it to the given NPCs inventory
		 * @param npc NPC to give the item to
		 */
        void pickUp(Handle::EntityHandle npc);

        /**
		 * @return Classes which want to get exported on save should return true here
		 */
        virtual bool shouldExport() { return true; }
        virtual void importObject(const json& j);

    protected:
        virtual void exportPart(json& j) override;

        struct
        {
            size_t scriptInstance;
        } m_ScriptState;
    };
}
