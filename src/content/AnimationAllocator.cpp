#include <utils/logger.h>
#include <vdfs/fileIndex.h>
#include <zenload/modelAnimationParser.h>
#include <zenload/zenParser.h>

#include "AnimationAllocator.h"

using namespace ZenLoad;

Animations::AnimationAllocator::AnimationAllocator(const VDFS::FileIndex *vdfidx)
    : m_pVDFSIndex(vdfidx)
{
}

Animations::AnimationAllocator::~AnimationAllocator()
{
}

Handle::AnimationHandle Animations::AnimationAllocator::loadAnimationVDF(const VDFS::FileIndex& idx, const std::string& lib, const std::string &overlay, const std::string& name)
{
    std::string qname = lib + '-' + overlay + '-' + name;

    // Check if this was already loaded
    auto it = m_AnimationsByName.find(qname);
    if (it != m_AnimationsByName.end())
        return it->second;

#if 0
    //LogInfo() << "New animation: " << qname;

    ZenLoad::zCModelAni zani;
    zani.setScale(1.0f / 100.0f);

    // order is important!

    // MSB seems to define common animations for overlays

    // 'HUMAN.MDS'
    std::string msb_file = lib + ".MSB";
    LogInfo() << "try MSB " << msb_file;
    if (idx.hasFile(msb_file))
    {
        if (!zani.loadMSB(idx, msb_file))
        {
            LogError() << "MSB load failed " << msb_file;
            return Handle::AnimationHandle::makeInvalidHandle();
        }
    }

    // MUMANS_MILITIA.MDS adds on top
    std::string overlay_msb_file = overlay + ".MSB";
    LogInfo() << "try overlay MSB " << overlay_msb_file;
    if (idx.hasFile(overlay_msb_file))
    {
        if (!zani.loadMSB(idx, overlay_msb_file))
        {
            LogError() << "overlay MSB load failed " << overlay_msb_file;
            return Handle::AnimationHandle::makeInvalidHandle();
        }
    }


    // the MAN adds stuff on top
    std::string man_file = qname + ".MAN";
    if (!idx.hasFile(man_file))
    {
        man_file = lib +  '-' + name + ".MAN";
        if (!idx.hasFile(man_file))
            man_file = std::string();
    }

    if (!man_file.empty())
    {
        if (!zani.loadMAN(idx, man_file))
            return Handle::AnimationHandle::makeInvalidHandle();
    }

/*
    if (!zani.loadMAN(idx, name + ".MAN"))
        return Handle::AnimationHandle::makeInvalidHandle();
*/
#endif


    Handle::AnimationHandle h = m_Allocator.createObject();
    Animation& aniObject = m_Allocator.getElement(h);

    loadMAN(aniObject, idx, name + ".MAN");

    m_AnimationsByName[qname] = h;

    return h;
}

Handle::AnimationHandle Animations::AnimationAllocator::loadAnimationVDF(const std::string& lib, const std::string &overlay, const std::string& name)
{
    if (!m_pVDFSIndex)
        return Handle::AnimationHandle::makeInvalidHandle();

    return loadAnimationVDF(*m_pVDFSIndex, lib, overlay, name);
}

bool Animations::AnimationAllocator::loadMAN(Animation &anim, const VDFS::FileIndex& idx, const std::string &name)
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
