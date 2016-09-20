#pragma once
#include <daedalus/DaedalusGameState.h>
#include <set>
#include "Controller.h"
#include "LogicDef.h"

namespace Logic
{
	class ItemController : public Controller
	{
	public:
		/**
		 * @param world World of the underlaying entity
		 * @param entity Entity owning this controller
		 */
		ItemController(World::WorldInstance& world, Handle::EntityHandle entity, Daedalus::GameState::ItemHandle scriptInstance);

		/**
		 * Updates this vob from the script-side values (visual, etc)
		 */
		void updateVobFromScript();

	protected:

		struct {
			Daedalus::GameState::ItemHandle scriptInstance;
		}m_ScriptState;

	};
}
