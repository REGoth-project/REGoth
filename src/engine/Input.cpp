#include "Input.h"

using namespace Engine;

namespace Keys
{
    // All keys mapped to ascii-characters
    const int PrintableBegin = 32;
    const int PrintableEnd = 93;  // Inclusive
}

ActionBinding::ActionBinding(ActionType actionType, bool isContinuous, bool isInverted)
    : actionType(actionType)
    , isContinuous(isContinuous)
    , isInverted(isInverted)
{
}

bool ActionBinding::operator<(const ActionBinding& other) const
{
    if (actionType < other.actionType)
        return true;
    if (other.actionType < actionType)
        return false;

    if (isContinuous < other.isContinuous)
        return true;
    if (other.isContinuous < isContinuous)
        return false;

    if (isInverted < other.isInverted)
        return true;

    return false;
}

void Action::setEnabled(bool enabled)
{
    isEnabled = enabled;
}

Action::Action(std::function<void(bool, float)> func)
    : function(func)
    , isEnabled(true)
{
}

std::multimap<ActionBinding, int /* key */> Input::actionBindingToKeyMap;
std::map<ActionBinding, int /*mouseButton*/> Input::actionBindingToMouseButtonMap;
std::map<ActionBinding, Input::MouseAxis> Input::actionBindingToMouseAxisMap;

std::multimap<ActionType, Action> Input::actionTypeToActionMap;

std::bitset<Input::NUM_KEYS> Input::keyState;
std::bitset<Input::NUM_KEYS> Input::keyTriggered;
std::vector<int32_t> Input::modsTriggered(Input::NUM_KEYS);

std::bitset<Input::NUM_MOUSEBUTTONS> Input::mouseButtonState;
std::bitset<Input::NUM_MOUSEBUTTONS> Input::mouseButtonTriggered;

float Input::axisPosition[static_cast<std::size_t>(Input::MouseAxis::Count)];
std::bitset<static_cast<std::size_t>(Input::MouseAxis::Count)> Input::mouseAxisState;
std::bitset<static_cast<std::size_t>(Input::MouseAxis::Count)> Input::mouseAxisTriggered;

float Input::mouseSensitivity = 50.0f;
Math::float2 Input::mousePosition = {0.0f, 0.0f};
bool Input::isMouseLocked = false;
std::function<void(bool /* lock */)> Input::mouseLockCallback;
float Input::windowHalfHeight;
float Input::windowHalfWidth;
std::string Input::frameTextInput;

Action* Input::RegisterAction(ActionType actionType, std::function<void(bool, float)> function)
{
    // Returns pointer to inserted Action object
    return &((*actionTypeToActionMap.insert(std::make_pair(actionType, Action(function)))).second);
}

void Input::clearActions()
{
    actionTypeToActionMap.clear();
}

bool Input::RemoveAction(ActionType actionType, Action* action)
{
    auto rangeIterators = actionTypeToActionMap.equal_range(actionType);
    bool hasActionBeenFound = false;
    for (auto it = rangeIterators.first; it != rangeIterators.second; ++it)
        if (&((*it).second) == action)
        {
            actionTypeToActionMap.erase(it);
            hasActionBeenFound = true;
            break;
        }

    return hasActionBeenFound;
}

Math::float2 Input::getMouseCoordinates()
{
    return mousePosition;
}

void Input::bindKey(int key, ActionType actionType, bool isContinuous, bool isInverted)
{
    actionBindingToKeyMap.emplace(ActionBinding(actionType, isContinuous, isInverted), key);
}

void Input::bindMouseButton(int mouseButton, ActionType actionType, bool isContinuous, bool isInverted)
{
    actionBindingToMouseButtonMap[ActionBinding(actionType, isContinuous, isInverted)] = mouseButton;
}

void Input::bindMouseAxis(MouseAxis mouseAxis, ActionType actionType, bool isContinuous, bool isInverted)
{
    actionBindingToMouseAxisMap[ActionBinding(actionType, isContinuous, isInverted)] = mouseAxis;
}

void Input::keyEvent(int key, int scancode, int action, int mods)
{
    if (key < 0 || key > Input::NUM_KEYS)
        return;

    if (KEY_ACTION_PRESS == action)
    {
        keyState[key] = true;
        keyTriggered[key] = true;
        modsTriggered[key] = mods;
    }
    else if (KEY_ACTION_REPEAT == action)
    {
        keyTriggered[key] = true;
        modsTriggered[key] = mods;
    }
    else if (KEY_ACTION_RELEASE == action)
    {
        keyState[key] = false;
        modsTriggered[key] = 0;
    }
}

void Input::mouseButtonEvent(int button, int action, int mods)
{
    if (KEY_ACTION_PRESS == action)
    {
        mouseButtonState[button] = true;
        mouseButtonTriggered[button] = true;
    }
    else if (KEY_ACTION_RELEASE == action)
    {
        mouseButtonState[button] = false;
    }
}

void Input::mouseMoveEvent(double xPos, double yPos)
{
    constexpr size_t cursorXIndex = static_cast<std::size_t>(MouseAxis::CursorX);
    constexpr size_t cursorYIndex = static_cast<std::size_t>(MouseAxis::CursorY);

    float x = static_cast<float>(xPos) / windowHalfWidth - 1.0f;
    float y = static_cast<float>(yPos) / windowHalfHeight - 1.0f;

    if (axisPosition[cursorXIndex] != x)
    {
        mouseAxisState[cursorXIndex] = true;
        mouseAxisTriggered[cursorXIndex] = true;
    }
    else
        mouseAxisState[cursorXIndex] = false;

    if (axisPosition[cursorYIndex] != y)
    {
        mouseAxisState[cursorYIndex] = true;
        mouseAxisTriggered[cursorYIndex] = true;
    }
    else
        mouseAxisState[cursorYIndex] = false;

    axisPosition[cursorXIndex] = x;
    axisPosition[cursorYIndex] = y;
}

void Input::scrollEvent(double xOffset, double yOffset)
{
    float x = static_cast<float>(xOffset);
    float y = static_cast<float>(yOffset);
    //    if(axisPosition[static_cast<std::size_t>(MouseAxis::ScrollX)] != x)
    //        mouseAxisState[static_cast<std::size_t>(MouseAxis::ScrollX)] = true;
    //    else
    //        mouseAxisState[static_cast<std::size_t>(MouseAxis::ScrollX)] = false;

    //    if(axisPosition[static_cast<std::size_t>(MouseAxis::ScrollY)] != y)
    //        mouseAxisState[static_cast<std::size_t>(MouseAxis::ScrollY)] = true;
    //    else
    //        mouseAxisState[static_cast<std::size_t>(MouseAxis::ScrollY)] = false;

    mouseAxisState[static_cast<std::size_t>(MouseAxis::ScrollX)] = true;
    mouseAxisState[static_cast<std::size_t>(MouseAxis::ScrollY)] = true;
    // Mouse wheel events do always trigger. Consider doing this for mouse cursor events too
    mouseAxisTriggered[static_cast<std::size_t>(MouseAxis::ScrollX)] = true;
    mouseAxisTriggered[static_cast<std::size_t>(MouseAxis::ScrollY)] = true;

    axisPosition[static_cast<std::size_t>(MouseAxis::ScrollX)] = x;
    axisPosition[static_cast<std::size_t>(MouseAxis::ScrollY)] = y;
}

void Input::windowSizeEvent(int width, int height)
{
    windowHalfWidth = static_cast<float>(width) / 2.0f;
    windowHalfHeight = static_cast<float>(height) / 2.0f;
}

void Input::setMouseLockCallback(std::function<void(bool)> callback)
{
    mouseLockCallback = callback;
}

void Input::fireBindings()
{
    for (const auto itBindingToKey : actionBindingToKeyMap)
    {
        //                             is key currently pressed   AND ( is continuous                    OR key has just been triggered )
        bool triggerAction = keyState.test(itBindingToKey.second) && (itBindingToKey.first.isContinuous || keyTriggered.test(itBindingToKey.second));
        // Key causes a constant intensity of 1.0, when pressed.
        float intensity = triggerAction ? 1.0f : 0.0f;
        // Invert intensity if isInverted is true
        intensity = itBindingToKey.first.isInverted ? -intensity : intensity;

        for (const auto& action : actionTypeToActionMap)
        {
            if (action.first == itBindingToKey.first.actionType && action.second.isEnabled)
            {
                action.second.function(triggerAction, intensity);
            }
        }

        /*auto rangeIterators = actionTypeToActionMap.equal_range(itBindingToKey.first.actionType);
        for(auto itAction = rangeIterators.first; itAction != rangeIterators.second; ++itAction)
            if(itAction->second.isEnabled)
            {
                itAction->second.function(triggerAction, intensity);
            }*/
    }

    clearTriggered();

    for (const auto itBindingToButton : actionBindingToMouseButtonMap)
    {
        bool triggerAction = mouseButtonState.test(itBindingToButton.second) && (itBindingToButton.first.isContinuous || mouseButtonTriggered.test(itBindingToButton.second));
        // Button causes a constant intensity of 1.0 when pressed
        float intensity = triggerAction ? 1.0f : 0.0f;
        // Invert intensity if isInverted is true
        intensity = itBindingToButton.first.isInverted ? -intensity : intensity;

        auto rangeIterators = actionTypeToActionMap.equal_range(itBindingToButton.first.actionType);
        for (auto itAction = rangeIterators.first; itAction != rangeIterators.second; ++itAction)
            if (itAction->second.isEnabled)
                itAction->second.function(triggerAction, intensity);

        mouseButtonTriggered[itBindingToButton.second] = false;
    }

    float deltaMouse[2] = {
        axisPosition[static_cast<std::size_t>(MouseAxis::CursorX)] - mousePosition.x,
        axisPosition[static_cast<std::size_t>(MouseAxis::CursorY)] - mousePosition.y};

    mousePosition.x = axisPosition[static_cast<std::size_t>(MouseAxis::CursorX)];
    mousePosition.y = axisPosition[static_cast<std::size_t>(MouseAxis::CursorY)];

    for (const auto itBindingToAxis : actionBindingToMouseAxisMap)
    {
        const size_t& mouseAxisIndex = static_cast<std::size_t>(itBindingToAxis.second);
        bool triggerAction = mouseAxisState.test(mouseAxisIndex) && (itBindingToAxis.first.isContinuous || mouseAxisTriggered.test(mouseAxisIndex));

        // Special care for mouse coordinates must be taken. Since screen coordinates dont make too much sense
        // we are passing delta values of the mouse position.
        float intensity;
        if (itBindingToAxis.second == MouseAxis::CursorX || itBindingToAxis.second == MouseAxis::CursorY)
        {
            // Mouse axis index is guaranteed to be either 0 or 1 due to the condition check above.
            intensity = mouseSensitivity * deltaMouse[mouseAxisIndex];
        }
        else
            // GLFW's mouse wheel implementiation provides deltas.
            intensity = axisPosition[mouseAxisIndex];

        // Pass the axis position as intensity, caring for invertion
        intensity = itBindingToAxis.first.isInverted ? -intensity : intensity;

        auto rangeIterators = actionTypeToActionMap.equal_range(itBindingToAxis.first.actionType);
        for (auto itAction = rangeIterators.first; itAction != rangeIterators.second; ++itAction)
            if (itAction->second.isEnabled)
                itAction->second.function(triggerAction, intensity);

        mouseAxisTriggered[mouseAxisIndex] = false;
    }
}

void Input::setMouseLock(bool mouseLock)
{
    if (mouseLock != isMouseLocked)
    {
        mouseLockCallback(mouseLock);
        isMouseLocked = mouseLock;
    }
}

void Input::getMouseState(Input::MouseState& ms)
{
    ms.m_mx = (int)((mousePosition.x + 1) * windowHalfWidth);
    ms.m_my = (int)((mousePosition.y + 1) * windowHalfHeight);
    ms.m_mz = (int)axisPosition[static_cast<std::size_t>(MouseAxis::ScrollY)];
    ms.m_buttons[0] = (uint8_t)mouseButtonState[0];
    ms.m_buttons[1] = (uint8_t)mouseButtonState[1];
    ms.m_buttons[2] = (uint8_t)mouseButtonState[2];
}

void Input::clearTriggered()
{
    // Reset all the keys, for text input
    for (int i = 0; i < NUM_KEYS; i++)
    {
        keyTriggered[i] = false;
        modsTriggered[i] = 0;
    }
}

std::string Input::getActualKeyName(int key)
{
    if (key >= Keys::PrintableBegin && key <= Keys::PrintableEnd)
    {
        return std::string(1, (char)key);
    }

    return "";
}

std::string Input::getFrameTextInput()
{
    std::string r = frameTextInput;
    frameTextInput.clear();
    return r;
}

ActionBindingAlive::ActionBindingAlive()
    : action(nullptr)
{
}

ActionBindingAlive::ActionBindingAlive(ActionBindingAlive&& other)
    : ActionBindingAlive()
{
    swap(*this, other);
}

void ActionBindingAlive::swap(ActionBindingAlive& a, ActionBindingAlive& b)
{
    std::swap(a.actionType, b.actionType);
    std::swap(a.action, b.action);
}

ActionBindingAlive::~ActionBindingAlive()
{
    if (action)
        Input::RemoveAction(actionType, action);
}

ActionBindingAlive::ActionBindingAlive(Engine::ActionType actionType, Engine::Action* action)
    : actionType(actionType)
    , action(action)
{
}

ActionBindingAlive& ActionBindingAlive::operator=(ActionBindingAlive&& other)
{
    {
        // disposes *this first
        ActionBindingAlive tempEmpty;
        swap(*this, tempEmpty);
    }
    swap(*this, other);
    return *this;
}
