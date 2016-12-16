#pragma once
#include <memory/StaticReferencedAllocator.h>
#include "World.h"
#include <vdfs/fileIndex.h>
#include <ui/View.h>
#include <bx/commandline.h>

namespace Engine
{
	const int MAX_NUM_WORLDS = 4;

	class BaseEngine
	{
	public:

        struct EngineArgs
        {
			EngineArgs() : cmdline(0, NULL) {}

            std::string gameBaseDirectory;
            std::string startupZEN;
			std::string testVisual;
			std::string modfile;
			bx::CommandLine cmdline;
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
		Handle::WorldHandle  addWorld(const std::string& worldFile, const std::string& savegame = "");

		/**
		 * Removes a world and everything inside
		 * @param world World to remove
		 */
		void removeWorld(Handle::WorldHandle world);

		/**
		 * @brief Frame update // TODO: Remove width and height
		 */
		void frameUpdate(double dt, uint16_t width, uint16_t m_height);

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

		/**
		 * @return Base-level UI-View. Parent of all other views.
		 */
		UI::View& getRootUIView() { return m_RootUIView; }

		/**
		 * Sets the path the engine is looking for files
		 * @param path New path
		 */
		void setContentBasePath(const std::string& path){ m_ContentBasePath = path; }

		/*+
         * @return The path where the engine is looking for content files
         */
		const std::string& getContentBasePath(){ return m_ContentBasePath; }

		/**
		 * @return Allocator for always present textures
		 */
		Textures::TextureAllocator& getEngineTextureAlloc(){ return m_EngineTextureAlloc; }
	protected:

		/**
		 * Called when a world was added
		 */
		virtual void onWorldCreated(Handle::WorldHandle world);
		virtual void onWorldRemoved(Handle::WorldHandle world){};

		/**
		 * Update-method for subclasses
		 */
		virtual void onFrameUpdate(double dt, uint16_t width, uint16_t height)=0;

		/**
		 * @brief Called to load archives into the main VDFS-Index
		 *		  Overwrite to load your own default archives
		 */
		virtual void loadArchives();

		/**
		 * Currently active world instances
		 */
		std::list<World::WorldInstance> m_WorldInstances;

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

		/**
		 * Base UI-View
		 */
		UI::View m_RootUIView;

		/**
		 * Allocator for always present textures
		 */
		Textures::TextureAllocator m_EngineTextureAlloc;

		/**
		 * Folder where the content is
		 */
		std::string m_ContentBasePath;
	};
}