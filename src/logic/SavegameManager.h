#pragma once 
#include <vector>
#include <string>

namespace Engine
{
    class GameEngine;

    namespace SavegameManager
    {
        /**
         * @param GameEngine-pointer
         */
        bool init(Engine::GameEngine& engine);

        /**
         * Assembles a list of all available savegame names. Every entry will correspond to an index number.
         * Empty names mean, that there is no save currently stored.
         * @return List of available saves
         */
        std::vector<std::string> gatherAvailableSavegames();

        /**
         * Checks if the given savegame-slot contains valid data
         * Note: Will check for a folder named "savegame_<i>" and regoth_save.json
         * @return Whether there is a savegame at the given slot
         */
        bool isSavegameAvailable(int idx);

        /**
         * Removes all data from a savegame.
         * Note: Will empty the files, but not remove them
         */
        void clearSavegame(int idx);

        /**
         * Searches for all valid worlds in the given savegame.
         * Note: An empty file does not qualify as valid world.
         * @return list of files containing all valid worlds as filenames.
         */
        std::vector<std::string> getSavegameWorlds(int idx);

        /**
         * @return Path to the savegame with the given index. 
         * Note: there is no check if that one actually exists!
         */
        std::string buildSavegamePath(int idx);

        struct SavegameInfo
        {
            SavegameInfo() :
                version(0)
            {}
            unsigned int version;
            std::string name; // Name to be displayed in the menus
            std::string world; // World the player is currently in. (Name only, no ".zen" or other extensions)
            std::size_t timePlayed; // Time played in seconds
            static constexpr unsigned int LATEST_KNOWN_VERSION = 1; // latest version. check where needed. increment when needed.
        };

        /**
         * Writes basic information about the given savegame, like the world the player currently is in
         * @param idx index of the savegame to write
         * @param information about the given savegame
         */
        bool writeSavegameInfo(int idx, const SavegameInfo& info);

        /**
         * Reads information about the given savegame, like the world the player is currently in
         * @param idx
         * @return Information about the given savegame
         */
        SavegameInfo readSavegameInfo(int idx);

        /**
         * Writes actual world-data into the given savegame
         * @param idx Index of the savegame
         * @param worldName Name of the world which is saved. No extensions, just the name.
         * @param data Data containing the world information to be saved. 
         * @return success
         */
        bool writeWorld(int idx, const std::string& worldName, const std::string& data);

        /**
         * Reads world-data for the world with the given name.
         * @param idx Index of the savegame
         * @param worldName Name of the world to load
         * @return Data of the given savegames world. Empty string of not found or no data
         */
        std::string readWorld(int idx, const std::string& worldName);

        /**
         * Builds the path to a saved worldfile from the given slot
         * @param idx Index of the savegame
         * @param worldName Name of the world to appear in the path
         * @return full path + filename to the given world
         */
        std::string buildWorldPath(int idx, const std::string& worldName);
    }
}
