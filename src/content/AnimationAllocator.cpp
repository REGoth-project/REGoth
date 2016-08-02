//
// Created by desktop on 28.07.16.
//

#include "AnimationAllocator.h"

Animations::AnimationAllocator::AnimationAllocator(const VDFS::FileIndex *vdfidx)
{

}

Animations::AnimationAllocator::~AnimationAllocator()
{

}

Handle::AnimationHandle Animations::AnimationAllocator::loadAnimationVDF(const VDFS::FileIndex& idx, const std::string& name)
{
    // Check if this was already loaded
    auto it = m_AnimationsByName.find(name);
    if (it != m_AnimationsByName.end())
        return (*it).second;

    ZenLoad::zCModelAni zani(name, idx, 1.0f / 100.0f);

    Handle::AnimationHandle h = m_Allocator.createObject();
    Animation& aniObject = m_Allocator.getElement(h);
    aniObject.animation = zani;

    m_AnimationsByName[name] = h;

    return h;
}

Handle::AnimationHandle Animations::AnimationAllocator::loadAnimationVDF(const std::string& name)
{
    if (!m_pVDFSIndex)
        return Handle::AnimationHandle::makeInvalidHandle();

    return loadAnimationVDF(*m_pVDFSIndex, name);
}
