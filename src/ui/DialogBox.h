#include "View.h"
#include <string>
#include <functional>

// HACK: Work around windows.h messing this up with its define
#ifdef DialogBox
#undef DialogBox
#endif

namespace UI
{
    class DialogBox : public View
    {
    public:
        DialogBox();

        ~DialogBox();

        /**
         * Updates/draws the UI-Views
         * @param dt time since last frame
         * @param mstate mouse-state
         */
        void update(double dt, entry::MouseState &mstate, Render::RenderConfig &config) override;

        /**
         * Calls script-routines to fill up the choices
         * @param Choices presented to the user
         * @param name Name of the person or thing we are interacting with
         */
        void fillChoices(const std::vector<std::string> &choices, const std::string &name = "");

        /**
         * @return Choice the user clicked the last time. -1 if none.
         */
        int getChoiceTaken(){ return m_ChoiceTaken; }


    private:

        /**
         * Choices presented to the player
         */
        std::vector<std::string> m_Choices;

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