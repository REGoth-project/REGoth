#pragma once

#include <handle/HandleDef.h>
#include <map>
#include <vector>
#include <string>
#include <content/Animation.h>
#include <memory/Config.h>
#include <handle/Handle.h>
#include "zenload/zCModelAni.h"
#include "zenload/zCModelScript.h"
#include "zenload/zCModelMeshLib.h"

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

        AnimationAllocator(World::WorldInstance *world);

        Handle::AnimationHandle allocate(const std::string &name);

        Handle::AnimationHandle getAnimation(const std::string &name) const;

        Animation& getAnimation(Handle::AnimationHandle h) { return m_Allocator.getElement(h); }

    private:

        Memory::StaticReferencedAllocator<Animation, Config::MAX_NUM_LEVEL_ANIMATIONS> m_Allocator;

        std::map<std::string, Handle::AnimationHandle> m_AnimationsByName;
    };

    inline Handle::AnimationHandle AnimationAllocator::allocate(const std::string &name)
    {
        auto h = m_Allocator.createObject();
        m_AnimationsByName[name] = h;
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

        AnimationDataAllocator(World::WorldInstance *world, const VDFS::FileIndex* vdfidx = nullptr);
        virtual ~AnimationDataAllocator();

        /**
        * @brief Sets the VDFS-Index to use
        */
        void setVDFSIndex(const VDFS::FileIndex* vdfidx) { m_pVDFSIndex = vdfidx; }

        /**
         * @brief Loads an animation from the given or stored VDFS-FileIndex
         */
        Handle::AnimationDataHandle loadAnimationVDF(const VDFS::FileIndex& idx, const std::string& name);

        Handle::AnimationDataHandle loadAnimationVDF(const std::string &name);

        /**
		 * @brief Returns the animation of the given handle
		 */
        AnimationData& getAnimationData(Handle::AnimationDataHandle h) { return m_Allocator.getElement(h); }

    protected:

        World::WorldInstance *m_World = nullptr;

        /**
         * @brief Textures by their set names. Note: If names are doubled, only the last loaded texture
         *		  can be found here
         */
        std::map<std::string, Handle::AnimationDataHandle> m_AnimationsByName;

        /**
		 * Data allocator
		 */
        Memory::StaticReferencedAllocator<AnimationData, Config::MAX_NUM_LEVEL_ANIMATION_DATAS> m_Allocator;

        /**
         * Pointer to a vdfs-index to work on (can be nullptr)
         */
        const VDFS::FileIndex* m_pVDFSIndex;

        bool loadMAN(AnimationData &data, const VDFS::FileIndex &idx, const std::string &name);
    };
}
