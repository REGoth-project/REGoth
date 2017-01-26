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

void ::Components::Actions::Logic::destroyPfxComponent(Components::PfxComponent& c)
{
    if(bgfx::isValid(c.m_ParticleVB))
        bgfx::destroyDynamicVertexBuffer(c.m_ParticleVB);
}

void ::Components::Actions::Animation::destroyAnimationComponent(AnimationComponent& c)
{
    delete c.m_AnimHandler;
}




