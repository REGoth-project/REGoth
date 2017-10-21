#include "Entities.h"
#include "AnimHandler.h"

using namespace Components;


void AnimationComponent::init(AnimationComponent& c)
{
    c.m_AnimHandler = new AnimHandler;
}
