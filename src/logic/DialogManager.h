#pragma once

#include <handle/HandleDef.h>
#include <daedalus/DaedalusGameState.h>
#include <daedalus/DaedalusDialogManager.h>
#include <json.hpp>
#include <audio/AudioWorld.h>

using json = nlohmann::json;

namespace World
{
    class WorldInstance;
}

namespace UI
{
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
        bool init();

        /**
         * Updates the boxes according to the coices taken by the user
         * @param dt time since last frame
         */
        void update(double dt);

        /**
         * Start dialog
         */
        void startDialog(Daedalus::GameState::NpcHandle target);

        /**
         * exit the dialog
         */
        void endDialog();

        /**
         * Displays a subtitle text
         * @param subtitle Text to display
         * @param self Name of the person saying that line
         * @param speaker the EntityHandle of the speaker
         */
        void displaySubtitle(const std::string& subtitle, const std::string& self);

        /**
         * Stops displaying whatever subtitle there currently is
         */
        void stopDisplaySubtitle();

        /**
         * set sound ticket to be able to identify sound for stopping
         * ticket safe to use identificator
         */
        void setSoundTicket(Utils::Ticket<World::AudioWorld> ticket) { m_SoundTicket = ticket; }

        /**
         * Cancels current Talk
         */
        void cancelTalk();

        /**
         * Called as callback from the AudioWorld
         */
        void onTalkSoundStopped(Handle::EntityHandle talker);

        /**
         * @return Whether a dialog is currently active
         */
        bool isDialogActive() { return m_DialogActive; }

        /**
         * @return Whether a someone is currently talking
         */
        bool isTalking() { return m_Talking; }

        /**
         * @return The NPC the hero is talking to
         */
        Daedalus::GameState::NpcHandle getTarget() {return m_Interaction.target; }

        /**
         * Removes all choices currently in the dialogbox
         */
        void clearChoices();

        /**
         * Adds a single choice to the box
         * @param entry Choice entry.
         */
        void addChoice(ChoiceEntry& entry);

        /**
         * Adds a single choice to the box.
         * The entry will get a new nr to be guaranteed to be on top.
         * @param entry Choice entry.
         */
        void addChoiceFront(ChoiceEntry& entry);

        /**
         * Sets whether the DialogManager is in in the SubDialog state
         */
        void setSubDialogActive(bool flag);

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

        /**
         * Called by the script when the interaction will end
         * and the DialogManager should be closed after the last ConversationMessage (if any)
         * @param self NPC who the player is talking to
         */
        void queueDialogEndEvent(Daedalus::GameState::NpcHandle target);

        // TODO: Probably move this into script-engine
        Daedalus::GameState::DaedalusDialogManager* getScriptDialogManager(){ return m_ScriptDialogMananger; }

        void exportDialogManager(json& h);
        void importDialogManager(const json& j);

        /**
         * Performs a choice selected by the user
         * @param choice Choice index to perform (m_Interaction.infos)
         */
        void performChoice(size_t choice);

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

        /**
         * Whether a subtitlebox is currently shown
         */
        bool m_Talking;

        /**
         * Whether a hero is inside a multiple choice test.
         * When true the queue will not be cleared and normal dialog options will not be added
         * This state is left when the script calls the script function Info_ClearChoices
         */
        bool m_SubDialogActive;

        /**
         * Can be used to cancel the current Dialog Sound, when IA_Close occurs
         */
        Utils::Ticket<World::AudioWorld> m_SoundTicket;
    };
}
