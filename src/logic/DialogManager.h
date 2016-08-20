#pragma once

#include <daedalus/DaedalusGameState.h>
#include <daedalus/DaedalusDialogManager.h>

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
            Daedalus::GameState::NpcHandle player;
            Daedalus::GameState::NpcHandle target;
            std::vector<Daedalus::GameState::InfoHandle> infos;
            std::vector<std::pair<size_t, size_t>> optionsSorted;
        } m_Interaction;

        /**
         * Scriptside dialog manager
         */
        Daedalus::GameState::DaedalusDialogManager* m_ScriptDialogMananger;
    };
}