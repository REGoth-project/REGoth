#pragma once

#include <daedalus/DaedalusGameState.h>
#include <daedalus/DaedalusDialogManager.h>
#include <json.hpp>
#include <logic/messages/EventMessage.h>

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

            // Indicates whether choice should be auto played
            bool important;

            static bool comparator(const ChoiceEntry& a, const ChoiceEntry& b){ return a.nr < b.nr; };
        };

        /**
         * Called after the world got initialized
         */
        bool init();

        /**
         * Updates the boxes according to the choices taken by the user
         * @param dt time since last frame
         */
        void update(double dt);

        /**
         * To be called when one of the given actions were triggered
         * @param action Input action
         */
        void onInputAction(UI::EInputAction action);

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
         * Cancels current Talk
         */
        void cancelTalk();

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
         * Sets the current Dialog Message. To be able to cancel it
         */
        void setCurrentMessage(std::shared_ptr<EventMessages::ConversationMessage> message);

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
         * @param target conversation partner
         */
        void updateChoices(Daedalus::GameState::NpcHandle target);

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

        /**
         * @return the subtitle box
         */
        UI::SubtitleBox& getSubtitleBox() { return *m_ActiveSubtitleBox; }

        /**
         * Called when an NPC is about to say something
         * @param self NPC talking
         * @param target NPC talking to
         * @param msg Message to say
         */
        void onAIOutput(Daedalus::GameState::NpcHandle self, Daedalus::GameState::NpcHandle target, const ZenLoad::oCMsgConversationData& msg);

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
         * Currently active subtitle box
         */
        UI::SubtitleBox* m_ActiveSubtitleBox;

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
            /**
             * Handle to the the current Dialogoption
             * Used to identify which Subchoices to show or to check if there are any
             */
            Daedalus::GameState::InfoHandle currentInfo;

            /**
             * Can be used to cancel the current Dialog Sound, when IA_Close occurs.
             */
            std::shared_ptr<EventMessages::ConversationMessage> currentDialogMessage;

            /**
             * Remember all already chosen important infos, for the current Dialog
             */
            std::set<Daedalus::GameState::InfoHandle> importantKnown;
        } m_Interaction;

        /**
         * Scriptside dialog manager
         */
        Daedalus::GameState::DaedalusDialogManager* m_ScriptDialogMananger;

        /**
         * Whether someone is talking or the DialogOptionBox is visible
         */
        bool m_DialogActive;

        /**
         * Whether the conditions of all choices will be reevaluated
         */
        bool m_ProcessInfos;

        /**
         * Whether a subtitlebox is currently shown
         */
        bool m_Talking;
    };
}
