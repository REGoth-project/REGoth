//
// Created by markus on 28.05.17.
//

#include "AsyncAction.h"
#include "BaseEngine.h"

Engine::AsyncAction::AsyncAction(JobType&& job) :
    m_Finished(false),
    m_Job(job)
{
}

Engine::AsyncAction::AsyncAction(const Engine::AsyncAction::JobType &job)  :
        m_Finished(false),
        m_Job(job)
{
}

bool Engine::AsyncAction::run(Engine::BaseEngine &engine)
{
    if (m_Finished)
        return true;
    m_Finished = m_Job(engine);
    return m_Finished;
}
