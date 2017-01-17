#include <algorithm>
#include <vdfs/fileIndex.h>
#include <zenload/modelScriptParser.h>
#include <zenload/zenParser.h>

#include "content/AnimationLibrary.h"

using namespace Animations;
using namespace VDFS;
using namespace ZenLoad;

namespace Animations
{

AnimationLibrary::AnimationLibrary(const FileIndex &index)
    : m_Index(index)
{

}

Handle::AnimationHandle Animations::AnimationLibrary::getAnimation(const std::string &mesh_lib, const std::string &overlay, const std::string &name)
{
    std::string qname = mesh_lib + '_' + overlay + '_' + name;

    auto it = m_AnimationsByName.find(qname);
    if (it != m_AnimationsByName.end())
        return it->second;

    qname = mesh_lib + '-' + name;

    it = m_AnimationsByName.find(qname);
    if (it != m_AnimationsByName.end())
        return it->second;

    return Handle::AnimationHandle();
}

bool AnimationLibrary::loadAnimations()
{
    // both .MDS and .MSB, where .MDS has precedence
    std::vector<std::string> msb_loaded;

    std::string ext_mds = ".mds";
    std::string ext_msb = ".msb";

    for (const FileInfo &fi : m_Index.getKnownFiles())
    {
        std::string fn = fi.fileName;
        std::transform(fn.begin(), fn.end(), fn.begin(), ::toupper);

        std::string n = fn.substr(0, fn.length() - 4);

        if (std::equal(ext_mds.rbegin(), ext_mds.rend(), fn.begin()))
        {
            if (!loadMDS(fn))
                return false;
        } else
        if (std::equal(ext_msb.rbegin(), ext_msb.rend(), fn.begin()))
        {
            if (!loadMSB(fn))
                return false;
        } else
            continue;

        msb_loaded.push_back(n);
    }

    return true;
}

bool AnimationLibrary::loadMDS(const std::string &file_name)
{
    return true;
}

bool AnimationLibrary::loadMSB(const std::string &file_name)
{
    ZenParser zen(file_name, m_Index);
    ModelScriptBinParser(zen);

    ModelScriptBinParser p(zen);
    ModelScriptBinParser::EChunkType type;
    while ((type = p.parse()) != ModelScriptBinParser::CHUNK_EOF)
    {
        switch (type)
        {
        case ModelScriptBinParser::CHUNK_ERROR:
            return false;
        }
    }

    return true;
}

} // namespace Animations
