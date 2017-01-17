#pragma once

#include <map>
#include <string>

#include <content/Animation.h>
#include <handle/HandleDef.h>
#include <memory/Config.h>

namespace Animations
{

class AnimationLibrary final
{
public:

    AnimationLibrary(const VDFS::FileIndex &index);

    Handle::AnimationHandle getAnimation(const std::string &name) const;

    Animation& getAnimation(Handle::AnimationHandle h) { return m_Allocator.getElement(h); }

    Handle::AnimationHandle getAnimation(const std::string &mesh_lib, const std::string &overlay, const std::string &name);

    bool loadAnimations();

private:

    const VDFS::FileIndex &m_Index;

    Memory::StaticReferencedAllocator<Animation, Config::MAX_NUM_LEVEL_ANIMATIONS> m_Allocator;

    std::map<std::string, Handle::AnimationHandle> m_AnimationsByName;

    bool loadMDS(const std::string &file_name);

    bool loadMSB(const std::string &file_name);
};

} // namespace Animations
