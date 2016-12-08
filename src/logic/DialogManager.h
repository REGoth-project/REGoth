#pragma once

#include <daedalus/DaedalusGameState.h>
#include <daedalus/DaedalusDialogManager.h>
#include <json.hpp>
using json = nlohmann::json;

namespace World
{
    class WorldInstance;
}

namespace UI
{
    class DialogBox;
    class PrintScreenMessages;
    class SubtitleBox;
}

namespace Logic
{

    class DialogManager
    {
    public:
        DialogManager(World::WorldInstance& world);
        ~DialogManager();

        struct ChoiceEntry
        {
            // Text displayed for the user
            std::string text;

            // Function symbol to be executed
            size_t functionSym;

            // Info-handle this belongs to
            Daedalus::GameState::InfoHandle info;

            // Sort index
            int32_t nr;
        };

        /**
         * Called after the world got initialized
         */
        void init();

        /**
         * Updates the boxes according to the coices taken by the user
         * @param dt time since last frame
         */
        void update(double dt);

        /**
         * Active dialog box accessors
         */
        UI::DialogBox* getActiveDialogBox() const
        { return m_ActiveDialogBox; }

        /**
         * Start dialog
         */
        void startDialog(Daedalus::GameState::NpcHandle target);

        /**
         * End dialog. Removes the dialogbox.
         */
        void endDialog();

        /**
         * Displays a subtitle text
         * @param subtitle Text to display
         * @param self Name of the person saying that line
         */
        void displaySubtitle(const std::string& subtitle, const std::string& self);

        /**
         * Stops displaying whatever subtitle there currently is
         */
        void stopDisplaySubtitle();

        /**
         * @return Whether a dialog is currently active
         */
        bool isDialogActive() { return m_DialogActive; }

        /**
         * Removes all choices currently in the dialogbox
         */
        void clearChoices();

        /**
         * Adds a single choice to the box
         * @param entry Choice entry. Note: nr of -1 means: Sort ascending
         * @return Index of the choice
         */
        size_t addChoice(ChoiceEntry& entry);

        /**
         * Sorts registered choices depending on their sort index
         */
        void sortChoices();

        /**
         * Pushes all registered choices to the dialog-box and opens it
         */
        void flushChoices();

        /**
         * Updates the choices from script-side. Restores the original set.
         */
        void updateChoices();

        // TODO: Probably move this into script-engine
        Daedalus::GameState::DaedalusDialogManager* getScriptDialogManager(){ return m_ScriptDialogMananger; }

        void exportDialogManager(json& h);
        void importDialogManager(const json& j);
    protected:

        /**
         * @return Daedalus script-VM
         */
        Daedalus::DaedalusVM& getVM();

        Daedalus::GameState::DaedalusGameState& getGameState();

        /**
         * Called right after the player started the interaction
         * @param self NPC who started the interaction
         * @param infos List of choices the player has to select
         */
        void onAIProcessInfos(Daedalus::GameState::NpcHandle self, std::vector<Daedalus::GameState::InfoHandle> infos);

        /**
         * Called when an NPC is about to say something
         * @param self NPC talking
         * @param target NPC talking to
         * @param msg Message to say
         */
        void onAIOutput(Daedalus::GameState::NpcHandle self, Daedalus::GameState::NpcHandle target, const ZenLoad::oCMsgConversationData& msg);

        /**
         * Performs a choice selected by the user
         * @param choice Choice index to perform (m_Interaction.infos)
         * @return Whether to continue the dialog (if false, END was selected)
         */
        bool performChoice(size_t choice);

        /**
         * Currently active dialog-box. nullptr, if none is active.
         */
        UI::DialogBox* m_ActiveDialogBox;

        /**
         * Currently active subtitle box
         */
        UI::SubtitleBox* m_ActiveSubtitleBox;

        /**
         * View for print-screen messages
         */
        UI::PrintScreenMessages* m_PrintScreenMessageView;

        /**
         * World this runs in
         */
        World::WorldInstance& m_World;

        /**
         * Information about the current interaction
         */
        struct
        {
            std::vector<ChoiceEntry> choices;

            Daedalus::GameState::NpcHandle player;
            Daedalus::GameState::NpcHandle target;
            std::vector<Daedalus::GameState::InfoHandle> infos;
            std::vector<size_t> functions;
            std::vector<std::pair<size_t, size_t>> optionsSorted;
        } m_Interaction;

        /**
         * Scriptside dialog manager
         */
        Daedalus::GameState::DaedalusDialogManager* m_ScriptDialogMananger;

        /**
         * Whether a dialog is currently active
         */
        bool m_DialogActive;
    };
}