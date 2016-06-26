#pragma once
#include <memory/StaticReferencedAllocator.h>
#include "World.h"
#include <vdfs/fileIndex.h>

namespace Engine
{
	const int MAX_NUM_WORLDS = 4;

	class BaseEngine
	{
	public:

        struct EngineArgs
        {
            std::string gameBaseDirectory;
            std::string startupZEN;
        };

		BaseEngine();
		virtual ~BaseEngine();

		/**
		 * @brief Initializes the Engine
		 */
		virtual void initEngine(int argc, char** argv);

		/**
		 * @brief Adds a world based of the given file
		 * @param worldfile Path to look for the worldfile. Can be inside a VDF-Archive
		 *		  or on disk (TODO)
		 */
		Handle::WorldHandle  addWorld(const std::string& worldFile);

		/**
		 * @brief Creates an empty world
		 */
		Handle::WorldHandle addWorld();

		/**
		 * @brief Frame update
		 */
		void frameUpdate(double dt);

		/**
		 * @return Main VDF-Archive
		 */
		VDFS::FileIndex& getVDFSIndex() { return m_FileIndex;  }

		/**
		 * Returns the world-instance of the given handle.
		 * Note: Do not save this pointer somewhere! It may change!
		 * @param h Handle to look up
		 * @return Data of the world-instance
		 */
		World::WorldInstance& getWorldInstance(Handle::WorldHandle& h);

        /**
         * @return Arguments passed to the engine
         */
        EngineArgs getEngineArgs();

	protected:

		/**
		 * Called when a world was added
		 */
		virtual void onWorldCreated(Handle::WorldHandle world);

		/**
		 * Update-method for subclasses
		 */
		virtual void onFrameUpdate(double dt)=0;

		/**
		 * @brief Called to load archives into the main VDFS-Index
		 *		  Overwrite to load your own default archives
		 */
		virtual void loadArchives();

		/**
		 * Currently active world instances
		 */
		Memory::StaticReferencedAllocator<World::WorldInstance, MAX_NUM_WORLDS> m_WorldInstances;

		/**
		 * Main VDFS-Index
		 */
		VDFS::FileIndex m_FileIndex;

		/**
		 * Registered worlds
		 */
		std::vector<Handle::WorldHandle> m_Worlds;

        /**
         * Arguments
         */
        EngineArgs m_Args;
	};
}