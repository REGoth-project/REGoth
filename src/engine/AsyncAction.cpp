//
// Created by markus on 28.05.17.
//

#include "AsyncAction.h"
#include "BaseEngine.h"

using namespace Engine;

bool Engine::AsyncAction::run(Engine::BaseEngine& engine)
{
    return m_Job(&engine);
}