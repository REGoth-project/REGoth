#pragma once
#include <vector>
#include <string>
#include <memory>
#include <json/json.hpp>

namespace Engine
{
    class GameEngine;

    namespace SavegameManager
    {
        /**
         * @param GameEngine-pointer
         */
        void init(Engine::GameEngine& engine);

        void invalidateCurrentSlotIndex();

        int getCurrentSlotIndex();

        /**
         * Assembles a list of all available savegame names. Every entry will correspond to an index number.
         * nullptr means, that there is no save currently stored.
         * @return List of available saves
         */
        std::vector<std::shared_ptr<const std::string>> gatherAvailableSavegames();

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
            SavegameInfo()
                : version(0)
            {
            }
            unsigned int version;
            std::string name;                                        // Name to be displayed in the menus
            std::string world;                                       // World the player is currently in. (Name only, no ".zen" or other extensions)
            std::size_t timePlayed;                                  // Time played in seconds
            static constexpr unsigned int LATEST_KNOWN_VERSION = 1;  // latest version. check where needed. increment when needed.
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
         * Writes actual player-data into the given savegame
         * @param idx Index of the savegame
         * @param playerName Name of the player which is saved. No extensions, just the name.
         * @param player as json object
         * @return success
         */
        bool writePlayer(int idx, const std::string& playerName, const nlohmann::json& world);

        /**
         * Reads player-data for the player with the given name.
         * @param idx Index of the savegame
         * @param playerName Name of the player to load
         * @return Data of the given savegame's player. Empty string if not found or no data
         */
        std::string readPlayer(int idx, const std::string& playerName);

        /**
         * Writes actual world-data into the given savegame
         * @param idx Index of the savegame
         * @param worldName Name of the world which is saved. No extensions, just the name.
         * @param world as json object
         * @return success
         */
        bool writeWorld(int idx, const std::string& worldName, const nlohmann::json& world);

        /**
         * Reads world-data for the world with the given name.
         * @param idx Index of the savegame
         * @param worldName Name of the world to load
         * @return Data of the given savegames world. Empty string if not found or no data
         */
        std::string readWorld(int idx, const std::string& worldName);

        /**
         * write the file with the specified filename to the given slot
         * @param idx slot index
         * @param relativePath filename relative to the savegame slot folder
         * @param file content to be saved.
         * @return success
         */
        bool writeFileInSlot(int idx, const std::string& relativePath, const std::string& data);

        /**
         * reads the file with the specified filename in the given slot
         * @param idx slot index
         * @param relativePath filename relative to the savegame slot folder
         */
        std::string readFileInSlot(int idx, const std::string& relativePath);

        /**
         * loads the savegame of the specified slotindex
         * @param index slotindex
         * @return empty string if successful, else error message
         */
        std::string loadSaveGameSlot(int index);

        /**
         * saves the current world to the given slot
         * @param index slotindex
         * @param savegameName label of the savegame. If empty string, then "Slot <index>" is used as name
         */
        void saveToSlot(int index, std::string savegameName);

        /**
         * Builds the path to a saved worldfile from the given slot
         * @param idx Index of the savegame
         * @param worldName Name of the world to appear in the path
         * @return full path + filename to the given world
         */
        std::string buildWorldPath(int idx, const std::string& worldName);

        constexpr int G1_MAX_SLOTS = 15 + 1;  // 15 usual slots + quicksave
        constexpr int G2_MAX_SLOTS = 20 + 1;  // 20 usual slots + quicksave

        std::string gameSpecificSubFolderName();

        int maxSlots();

        enum SaveGameActionType
        {
            Save
        };

        struct SaveGameAction
        {
            SaveGameActionType type;
            int slot;
            std::string savegameName;
        };
    }
}
