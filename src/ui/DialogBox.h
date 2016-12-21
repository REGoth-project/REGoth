#include "View.h"
#include <string>
#include <functional>
#include <logic/DialogManager.h>

// HACK: Work around windows.h messing this up with its define
#ifdef DialogBox
#undef DialogBox
#endif

namespace UI
{
    class DialogBox : public View
    {
    public:
        DialogBox(Engine::BaseEngine& e);

        ~DialogBox();

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig &config) override;

        /**
         * Adds a single choice
         * @param choice Choice presented to the user
         * @param name Name of the person or thing we are interacting with
         */
        size_t addChoice(Logic::DialogManager::ChoiceEntry& entry);

        /**
         * @return Choice the user clicked the last time. -1 if none.
         */
        int getChoiceTaken(){ return m_ChoiceTaken; }

        /**
         * @return List of choices registered
         */
        const std::vector<Logic::DialogManager::ChoiceEntry>& getChoices(){ return m_Choices; }

    private:

        /**
         * Choices presented to the player
         */
        std::vector<Logic::DialogManager::ChoiceEntry> m_Choices;

        /**
         * Choice the user clicked the last time. -1 if none.
         */
        int m_ChoiceTaken;

        /**
         * Current state of the scroll-area
         */
        int32_t m_ScrollArea;

        /**
         * Name of the thing or person we are interacting with
         */
        std::string m_TargetName;
    };
}