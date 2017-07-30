#pragma once

#include <map>
#include <string>
#include <vector>
#include "zenload/zCModelAni.h"
#include "zenload/zCModelMeshLib.h"
#include "zenload/zCModelScript.h"
#include <content/Animation.h>
#include <handle/Handle.h>
#include <handle/HandleDef.h>
#include <memory/Config.h>

namespace VDFS
{
    class FileIndex;
}

namespace World
{
    class WorldInstance;
}

namespace ZenLoad
{
    class zCModelMeshLib;
}

namespace Animations
{
    class AnimationAllocator
    {
    public:
        Handle::AnimationHandle allocate(const std::string &name);

        Handle::AnimationHandle getAnimation(const std::string &name) const;

        Animation &getAnimation(Handle::AnimationHandle h) { return m_Allocator.getElement(h); }
    private:
        Memory::StaticReferencedAllocator<Animation, Config::MAX_NUM_LEVEL_ANIMATIONS> m_Allocator;

        std::map<std::string, Handle::AnimationHandle> m_AnimationsByName;
    };

    inline Handle::AnimationHandle AnimationAllocator::allocate(const std::string &name)
    {
        std::string uname = name;
        std::transform(uname.begin(), uname.end(), uname.begin(), ::toupper);

        auto h = m_Allocator.createObject();
        m_AnimationsByName[uname] = h;
        return h;
    }

    inline Handle::AnimationHandle AnimationAllocator::getAnimation(const std::string &name) const
    {
        auto it = m_AnimationsByName.find(name);
        return (it != m_AnimationsByName.end()) ? it->second : Handle::AnimationHandle();
    }

    class AnimationDataAllocator
    {
    public:
        Handle::AnimationDataHandle allocate(const std::string &name);

        AnimationData &getAnimationData(Handle::AnimationDataHandle h) { return m_Allocator.getElement(h); }
        Handle::AnimationDataHandle getAnimationData(const std::string &name);

    protected:
        std::map<std::string, Handle::AnimationDataHandle> m_AnimationDataByName;

        Memory::StaticReferencedAllocator<AnimationData, Config::MAX_NUM_LEVEL_ANIMATION_DATAS> m_Allocator;
    };

    inline Handle::AnimationDataHandle AnimationDataAllocator::allocate(const std::string &name)
    {
        auto h = m_Allocator.createObject();
        m_AnimationDataByName[name] = h;
        return h;
    }

    inline Handle::AnimationDataHandle AnimationDataAllocator::getAnimationData(const std::string &name)
    {
        auto it = m_AnimationDataByName.find(name);
        return (it != m_AnimationDataByName.end()) ? it->second : Handle::AnimationDataHandle();
    }
}
