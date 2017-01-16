#pragma once

#include <handle/HandleDef.h>

namespace Animations
{

class AnimationLibrary final
{
public:

    Handle::AnimationHandle getAnimation(const std::string &name, const std::string &overlay) const;
};

}
