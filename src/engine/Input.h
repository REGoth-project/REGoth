#pragma once
#include <GLFW/glfw3.h>
#include <bitset>
#include <functional>
#include <map>
#include <unordered_map>
#include <type_traits>
#include <cstring>

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

    FirstPersonMoveForward,
    FirstPersonMoveRight,
    FirstPersonLookHorizontal,
    FirstPersonLookVertical,

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
    PlayerStrafeLeft,
    PlayerStrafeRight,
    PlayerDrawWeaponMelee,

    AllNpcDrawWeapon,
    AllNpcUndrawWeapon,
    AllNpcAttackFront,

    DebugSkySpeed,

    Count
};

struct ActionBinding
{
    ActionBinding(ActionType actionType, bool isContinuous, bool isInverted);
    bool operator<(const ActionBinding &other) const;

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

class Input
{
public:
    enum class MouseAxis
    {
        CursorX = 0,
        CursorY,
        ScrollX,
        ScrollY,
        Count
    };

public:
    static Action* RegisterAction( ActionType actionType, std::function<void(bool /*triggered*/, float /*intensity*/)> function);
    static bool RemoveAction(ActionType actionType, Action* action);
    static void fireBindings();
    static void setMouseLock(bool mouseLock);
    static Math::float2 getMouseCoordinates();

protected:
    static void bindKey(int key, ActionType actionType, bool isContinuous, bool isInverted = false);
    static void bindMouseButton(int mouseButton, ActionType actionType, bool isContinuous, bool isInverted = false);
    static void bindMouseAxis(MouseAxis mouseAxis, ActionType actionType, bool isContinuous, bool isInverted = false);

    static void keyEvent(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void mouseButtonEvent(GLFWwindow *window, int button, int action, int mods);
    static void mouseMoveEvent(GLFWwindow *window, double xPos, double yPos);
    static void scrollEvent(GLFWwindow *window, double xOffset, double yOffset);

    static void windowSizeEvent(GLFWwindow *window, int width, int height);

    static void setMouseLockCallback(std::function<void(bool /* lock */)> callback);

private:
    static std::map<ActionBinding, int /* key */> actionBindingToKeyMap;
    static std::map<ActionBinding, int /*mouseButton*/> actionBindingToMouseButtonMap;
    static std::map<ActionBinding, MouseAxis> actionBindingToMouseAxisMap;

    static std::multimap<ActionType, Action> actionTypeToActionMap;

    static std::bitset<GLFW_KEY_LAST + 1> keyState;
    static std::bitset<GLFW_KEY_LAST + 1> keyTriggered;

    static std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> mouseButtonState;
    static std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> mouseButtonTriggered;

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
