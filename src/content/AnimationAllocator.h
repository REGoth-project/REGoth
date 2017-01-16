#pragma once

#include <handle/HandleDef.h>
#include <map>
#include <vector>
#include <string>
#include <memory/Config.h>
#include <handle/Handle.h>
#include "zenload/zCModelAni.h"
#include "zenload/zCModelScript.h"
#include "zenload/zCModelMeshLib.h"

namespace VDFS
{
    class FileIndex;
}

namespace ZenLoad
{
    class zCModelMeshLib;
}

namespace Animations
{
    struct Animation : public Handle::HandleTypeDescriptor<Handle::AnimationHandle>
    {

        ZenLoad::zCModelAniHeader               m_Header;
        std::vector<ZenLoad::zCModelAniSample>  m_Samples;
        std::vector<uint32_t>                   m_NodeIndexList;
    };

    class AnimationAllocator
    {
    public:

        AnimationAllocator(const VDFS::FileIndex* vdfidx = nullptr);
        virtual ~AnimationAllocator();

        /**
        * @brief Sets the VDFS-Index to use
        */
        void setVDFSIndex(const VDFS::FileIndex* vdfidx) { m_pVDFSIndex = vdfidx; }

        /**
         * @brief Loads an animation from the given or stored VDFS-FileIndex
         */
        Handle::AnimationHandle loadAnimationVDF(const VDFS::FileIndex& idx, const std::string& lib, const std::string& overlay, const std::string& name);

        Handle::AnimationHandle loadAnimationVDF(const std::string& lib, const std::string& overlay, const std::string &name);

        /**
		 * @brief Returns the animation of the given handle
		 */
        Animation& getAnimation(Handle::AnimationHandle h) { return m_Allocator.getElement(h); }
    protected:
        /**
         * @brief Textures by their set names. Note: If names are doubled, only the last loaded texture
         *		  can be found here
         */
        std::map<std::string, Handle::AnimationHandle> m_AnimationsByName;

        /**
		 * Data allocator
		 */
        Memory::StaticReferencedAllocator<Animation, Config::MAX_NUM_LEVEL_ANIMATIONS> m_Allocator;

        /**
         * Pointer to a vdfs-index to work on (can be nullptr)
         */
        const VDFS::FileIndex* m_pVDFSIndex;

        bool loadMAN(Animation &anim, const VDFS::FileIndex &idx, const std::string &name);
    };
}
