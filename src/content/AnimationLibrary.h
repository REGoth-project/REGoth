#pragma once

#include <map>
#include <string>

#include <content/Animation.h>
#include <handle/HandleDef.h>
#include <memory/Config.h>

namespace ZenLoad
{

class ModelScriptParser;

}

namespace Animations
{

class AnimationLibrary final
{
public:

    AnimationLibrary(World::WorldInstance &world);

    Animation& getAnimation(Handle::AnimationHandle h);

    Handle::AnimationHandle getAnimation(const std::string &qname);

    Handle::AnimationHandle getAnimation(const std::string &mesh_lib, const std::string &overlay, const std::string &name);

    AnimationData& getAnimationData(Handle::AnimationDataHandle h);

    Handle::AnimationHandle getAnimationData(const std::string &name);

    bool loadAnimations();

    static std::string makeQualifiedName(const std::string &mesh_lib, const std::string &overlay, const std::string &name);

private:

    World::WorldInstance &m_World;

    bool loadMDS(const std::string &file_name);

    bool loadModelScript(const std::string &file_name, ZenLoad::ModelScriptParser &mds);

    Handle::AnimationDataHandle loadMAN(const std::string &name);

    // resolves referenced handles (aliases, next anis)
    void resolve();
};

} // namespace Animations
