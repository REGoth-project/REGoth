//
// Created by markus on 28.05.17.
//

#include "AsyncAction.h"
#include "GameEngine.h"

using namespace Engine;

bool Engine::AsyncAction::run(Engine::GameEngine& engine)
{
    return m_Job(&engine);
}
