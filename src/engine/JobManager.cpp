#include "JobManager.h"
#include <cassert>

namespace Engine
{
JobManager::JobManager(Engine::BaseEngine* engine, bool enableMultiThreading)
    : m_EnableMultiThreading(enableMultiThreading)
    , m_ThreadId(std::this_thread::get_id())
    , m_pEngine(engine)
{
}
bool JobManager::isSameThread()
{
    return std::this_thread::get_id() == m_ThreadId;
}

void JobManager::setMultiThreading(bool enable)
{
    m_EnableMultiThreading = enable;
}

void JobManager::queueJob(JobType<void> job)
{
    std::lock_guard<std::recursive_mutex> guard(m_JobQueueMutex);
    m_JobQueue.push_back(std::move(job));
}

void JobManager::processJobs()
{
    assert(isSameThread());

    // execute all pending synchronous jobs
    {
        std::lock_guard<std::recursive_mutex> guard(m_JobQueueMutex);
        while (!m_JobQueue.empty())
        {
            auto& job = m_JobQueue.front();
            // the job may queue more jobs, that's why a recursive_mutex is used
            job(m_pEngine);
            m_JobQueue.pop_front();
        }
    }

    // rethrow exceptions from other threads, remove finished jobs from keep-alive-list
    {
        std::lock_guard<std::mutex> lock(m_AsyncJobsMutex);
        auto it = m_AsyncJobs.begin();
        while (it != m_AsyncJobs.end())
        {
            // test if execution finished
            bool finished = it->wait_for(std::chrono::nanoseconds(0)) == std::future_status::ready;
            if (finished)
            {
                // calling .get() will rethrow any exception, that occurred while executing the future
                it->get();
            }
            if (finished)
                it = m_AsyncJobs.erase(it);  // erase returns next iterator
            else
                std::advance(it, 1);
        }
    }
}
}

