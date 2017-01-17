#include <content/AnimationAllocator.h>
#include <engine/World.h>
#include <utils/logger.h>
#include <vdfs/fileIndex.h>
#include <zenload/modelAnimationParser.h>
#include <zenload/zenParser.h>

#include "AnimationAllocator.h"

using namespace ZenLoad;

namespace Animations
{

AnimationDataAllocator::AnimationDataAllocator(World::WorldInstance *world, const VDFS::FileIndex *vdfidx)
    : m_World(world),
      m_pVDFSIndex(vdfidx)
{
}

AnimationDataAllocator::~AnimationDataAllocator()
{
}

Handle::AnimationDataHandle AnimationDataAllocator::loadAnimationVDF(const VDFS::FileIndex& idx, const std::string& name)
{
    // Check if this was already loaded
    auto it = m_AnimationsByName.find(name);
    if (it != m_AnimationsByName.end())
        return it->second;

    Handle::AnimationDataHandle h = m_Allocator.createObject();
    AnimationData& data = m_Allocator.getElement(h);

    loadMAN(data, idx, name + ".MAN");

    m_AnimationsByName[name] = h;

    return h;
}

Handle::AnimationDataHandle AnimationDataAllocator::loadAnimationVDF(const std::string& name)
{
    if (!m_pVDFSIndex)
        return Handle::AnimationDataHandle::makeInvalidHandle();

    return loadAnimationVDF(*m_pVDFSIndex, name);
}

bool AnimationDataAllocator::loadMAN(AnimationData &data, const VDFS::FileIndex& idx, const std::string &name)
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
            {
                const zCModelAniHeader &h = p.header();
                anim.m_Name = h.aniName;
                anim.m_Layer = h.layer;
                anim.m_FrameCount = h.numFrames;
                anim.m_FpsRate = h.fpsRate;
            }
            break;

        case ModelAnimationParser::CHUNK_RAWDATA:
            {
                anim.m_NodeIndex = p.nodeIndex();
                anim.m_Samples = p.samples();
                anim.m_Data
            }
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

