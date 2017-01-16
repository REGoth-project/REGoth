#include <utils/logger.h>
#include <vdfs/fileIndex.h>
#include <zenload/modelAnimationParser.h>
#include <zenload/zenParser.h>

#include "AnimationAllocator.h"

using namespace ZenLoad;

namespace Animations
{

AnimationDataAllocator::AnimationAllocator(const VDFS::FileIndex *vdfidx)
    : m_pVDFSIndex(vdfidx)
{
}

AnimationDataAllocator::~AnimationAllocator()
{
}

Handle::AnimationHandle AnimationDataAllocator::loadAnimationVDF(const VDFS::FileIndex& idx, const std::string& name)
{
    // Check if this was already loaded
    auto it = m_AnimationsByName.find(name);
    if (it != m_AnimationsByName.end())
        return it->second;

    Handle::AnimationHandle h = m_Allocator.createObject();
    Animation& aniObject = m_Allocator.getElement(h);

    loadMAN(aniObject, idx, name + ".MAN");

    m_AnimationsByName[name] = h;

    return h;
}

Handle::AnimationHandle AnimationDataAllocator::loadAnimationVDF(const std::string& name)
{
    if (!m_pVDFSIndex)
        return Handle::AnimationHandle::makeInvalidHandle();

    return loadAnimationVDF(*m_pVDFSIndex, name);
}

bool AnimationDataAllocator::loadMAN(Animation &anim, const VDFS::FileIndex& idx, const std::string &name)
{
    if (!idx.hasFile(name))
    {
        LogError() << "MAN file " <<name << " not found";
        return false;
    }

    ZenParser zenp(name, idx);

    ModelAnimationParser p(zenp);
    p.setScale(1.0f / 100.0f);

    ModelAnimationParser::EChunkType type;
    while ((type = p.parse()) != ModelAnimationParser::CHUNK_EOF)
    {
        switch (type)
        {
        case ModelAnimationParser::CHUNK_HEADER:
            anim.m_Header = p.header();
            break;

        case ModelAnimationParser::CHUNK_RAWDATA:
            anim.m_NodeIndexList = p.nodeIndex();
            anim.m_Samples = p.samples();
            break;

        case ModelAnimationParser::CHUNK_ERROR:
            return false;
        default:
            break;
        }
    }
    return true;
}

}

