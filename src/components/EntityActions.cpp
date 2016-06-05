#include "EntityActions.h"
#include <logic/Controller.h>
#include <logic/VisualController.h>

void ::Components::Actions::Logic::destroyLogicComponent(Components::LogicComponent& c)
{
    delete c.m_pLogicController;
}

void ::Components::Actions::Logic::destroyVisualComponent(VisualComponent& c)
{
    delete c.m_pVisualController;
}




