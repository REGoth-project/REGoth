#pragma once
#include <bitset>
#include <cstdint>
#include <cstring>
#include <functional>
#include <map>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "math/mathlib.h"

namespace Engine
{
    enum class ActionType
    {
        PauseGame = 0,

        CameraFirstPerson,
        CameraFree,
        CameraViewer,
        CameraThirdPerson,

        DebugMoveSpeed,
        DebugMoveSpeed2,

        FirstPersonMoveForward,
        FirstPersonMoveRight,
        FirstPersonLookHorizontal,
        FirstPersonLookVertical,

        ThirdPersonLookHorizontal,
        ThirdPersonLookVertical,
        ThirdPersonMouseWheel,

        FreeMoveForward,
        FreeMoveRight,
        FreeMoveUp,
        FreeLookHorizontal,
        FreeLookVertical,

        ViewerHorizontal,
        ViewerVertical,
        ViewerPan,
        ViewerZoom,
        ViewerRotate,
        ViewerClick,
        ViewerMouseWheel,

        PlayerForward,
        PlayerBackward,
        PlayerTurnLeft,
        PlayerTurnRight,
        PlayerRotate,
        PlayerStrafeLeft,
        PlayerStrafeRight,
        PlayerDrawWeaponMelee,
        PlayerAction,
        PlayerActionContinous,

        AllNpcDrawWeapon,
        AllNpcUndrawWeapon,
        AllNpcAttackFront,

        DebugSkySpeed,

        Quicksave,
        Quickload,
        Escape,
        UI_Backspace,
        UI_Up,
        UI_Down,
        UI_Close,
        UI_Left,
        UI_Right,
        UI_Confirm,
        UI_Mousewheel,
        UI_ToggleConsole,
        UI_ToggleLogMenu,
        UI_ToggleStatusMenu,
        UI_0,
        UI_1,
        UI_2,
        UI_3,
        UI_4,
        UI_5,
        UI_6,
        UI_7,
        UI_8,
        UI_9,
        UI_HOME,
        UI_END,

        Count
    };

    struct ActionBinding
    {
        ActionBinding(ActionType actionType, bool isContinuous, bool isInverted);
        bool operator<(const ActionBinding& other) const;

        ActionType actionType;
        bool isContinuous;
        bool isInverted;
    };

    class Action
    {
        friend class Input;

    public:
        void setEnabled(bool enabled);

    private:
        Action(std::function<void(bool /*triggered*/, float /*intensity*/)> func);

        bool isEnabled;
        std::function<void(bool /*triggered*/, float /*intensity*/)> function;
    };

    class ManagedActionBinding
    {
    public:
        ManagedActionBinding();
        ManagedActionBinding(Engine::ActionType actionType, Engine::Action* action);
        ManagedActionBinding(ManagedActionBinding&& other);
        ManagedActionBinding& operator= (ManagedActionBinding&& other);
        // copying is forbidden
        ManagedActionBinding(const ManagedActionBinding&) = delete;
        ManagedActionBinding& operator= (const ManagedActionBinding&) = delete;

        ~ManagedActionBinding();
        static void swap(ManagedActionBinding& a, ManagedActionBinding& b);
        Engine::Action& getAction() { return *action; }
    protected:
        Engine::ActionType actionType;
        Engine::Action* action;
    };

    class Input
    {
    public:
        static const int NUM_KEYS = 349;
        static const int NUM_MOUSEBUTTONS = 8;
        static const int KEY_ACTION_RELEASE = 0;
        static const int KEY_ACTION_PRESS = 1;
        static const int KEY_ACTION_REPEAT = 2;

        enum class MouseAxis
        {
            CursorX = 0,
            CursorY,
            ScrollX,
            ScrollY,
            Count
        };

        struct MouseState
        {
            int32_t m_mx;
            int32_t m_my;
            int32_t m_mz;
            uint8_t m_buttons[3];
        };

    public:
        /**
         * Registers a function to the given action
         * Caution: The returned object must be stored somewhere.
         * As soon as it goes out of scope, the binding is unregistered.
         * @return An object on whose end of lifetime the binding is automatically unregistered
         */
        static ManagedActionBinding RegisterAction(ActionType actionType, std::function<void(bool /*triggered*/, float /*intensity*/)> function);
        static bool RemoveAction(ActionType actionType, Action* action);
        static void clearActions();
        static void fireBindings();
        static void setMouseLock(bool mouseLock);
        static Math::float2 getMouseCoordinates();
        static void getMouseState(MouseState& ms);

        /**
         * Returns and clears the text input from the current frame
         * @return Text-Input since the last frame
         */
        static std::string getFrameTextInput();

        /**
         * Looks up the actual key to the given scancode. Libraries like GLFW work with mapping everything to a US-Layout,
         * which means that pressing the key that is on the location of the US Y key, for example, will always result in the
         * scancode of Y to be returned.
         * That is a problem on QWERTZ or AZERTY keyboards. This function returns a sequence of chars
         * which contain the name of the actual pressed key.
         * @param scancode Scancode from input-library (GLFW)
         * @return Actual name of the pressed key
         */
        virtual std::string getActualKeyName(int key);

    protected:
        static void bindKey(int key, ActionType actionType, bool isContinuous, bool isInverted = false);
        static void bindMouseButton(int mouseButton, ActionType actionType, bool isContinuous, bool isInverted = false);
        static void bindMouseAxis(MouseAxis mouseAxis, ActionType actionType, bool isContinuous, bool isInverted = false);

        static void keyEvent(int key, int scancode, int action, int mods);
        static void mouseButtonEvent(int button, int action, int mods);
        static void mouseMoveEvent(double xPos, double yPos);
        static void scrollEvent(double xOffset, double yOffset);
        static void windowSizeEvent(int width, int height);

        static void setMouseLockCallback(std::function<void(bool /* lock */)> callback);

        static const std::bitset<NUM_KEYS>& getKeysTriggered() { return keyTriggered; }
        static const std::vector<int32_t>& getModsTriggered() { return modsTriggered; }
        static const std::bitset<NUM_KEYS>& getKeysState() { return keyState; }
        static void clearTriggered();

        static std::string frameTextInput;

    private:
        static std::multimap<ActionBinding, int /* key */> actionBindingToKeyMap;
        static std::map<ActionBinding, int /*mouseButton*/> actionBindingToMouseButtonMap;
        static std::map<ActionBinding, MouseAxis> actionBindingToMouseAxisMap;

        static std::multimap<ActionType, Action> actionTypeToActionMap;

        static std::bitset<NUM_KEYS> keyState;
        static std::bitset<NUM_KEYS> keyTriggered;
        static std::vector<int32_t> modsTriggered;

        static std::bitset<NUM_MOUSEBUTTONS> mouseButtonState;
        static std::bitset<NUM_MOUSEBUTTONS> mouseButtonTriggered;

        static float axisPosition[static_cast<std::size_t>(MouseAxis::Count)];
        static std::bitset<static_cast<std::size_t>(MouseAxis::Count)> mouseAxisState;
        static std::bitset<static_cast<std::size_t>(MouseAxis::Count)> mouseAxisTriggered;

        static float mouseSensitivity;
        static Math::float2 mousePosition;
        static bool isMouseLocked;
        static std::function<void(bool /* lock */)> mouseLockCallback;
        static float windowHalfWidth;
        static float windowHalfHeight;
    };
}
