#include "EntityActions.h"
#include <logic/Controller.h>
#include <logic/VisualController.h>
#include <components/AnimHandler.h>

void ::Components::Actions::Logic::destroyLogicComponent(Components::LogicComponent& c)
{
    delete c.m_pLogicController;
    c.m_pLogicController = nullptr;
}

void ::Components::Actions::Logic::destroyVisualComponent(VisualComponent& c)
{
    delete c.m_pVisualController;
    c.m_pVisualController = nullptr;
}

void ::Components::Actions::Logic::destroyPfxComponent(Components::PfxComponent& c)
{
    if (bgfx::isValid(c.m_ParticleVB))
    {
        bgfx::destroy(c.m_ParticleVB);
        c.m_ParticleVB = BGFX_INVALID_HANDLE;
    }
}

void ::Components::Actions::Animation::destroyAnimationComponent(AnimationComponent& c)
{
    delete c.m_AnimHandler;
    c.m_AnimHandler = nullptr;
}
