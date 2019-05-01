#pragma once
#include <functional>
#include <future>
#include <list>
#include <mutex>
#include <thread>

namespace Engine
{
    class BaseEngine;

    enum class ExecutionPolicy
    {
        MainThread,
        NewThread
    };


template <class ReturnType>
using JobType = std::function<ReturnType(Engine::BaseEngine*)>;

class JobManager
{
public:
    JobManager(Engine::BaseEngine* engine, bool enableMultiThreading = true);

    /**
     * @return true if the calling thread is the same thread as the JobManager's creation thread
     */
    bool isSameThread();

    /**
     * if disabled, all asynchronous operations will be executud synchronously instead
     */
    void setMultiThreading(bool enable);

    /**
     * Guarantees execution of the given function in the JobManager's thread
     * May be called from any thread
     * @param job function to execute in the JobManager's thread
     */
    void queueJob(JobType<void> job);

    /**
     * executes all jobs in the queue, must be called from the JobManager's creation thread
     */
    void processJobs();

    /**
     * Executes the job in the specified thread
     * May be called from any thread
     * @param job the function to be executed
     * @param executionPolicy defines which thread should execute the job
     * @return future, which will contain the result
     */
    template <class ReturnType = void>
    std::future<ReturnType> executeInThread(JobType<ReturnType> job, ExecutionPolicy policy);

    template <class ReturnType = void>
    std::future<ReturnType> executeInMainThread(JobType<ReturnType> job)
    {
        return executeInThread(std::move(job), ExecutionPolicy::MainThread);
    }

    /**
     * Whether multi-threading is enabled
     */
    bool m_EnableMultiThreading;

protected:
    /**
     * wraps the job into a new functor, that puts the job's return value into the promise
     */
    template <class ReturnType>
    JobType<void> wrapJob(JobType<ReturnType> job, std::shared_ptr<std::promise<ReturnType>> promise);

    /**
     * ID of the current thread (i.e. bgfx thread)
     */
    const std::thread::id m_ThreadId;

    Engine::BaseEngine* const m_pEngine;

    /**
     * list of jobs to be executed at frame-end
     */
    std::list<JobType<void>> m_JobQueue;

    /**
     * This mutex must be locked when modifying m_jobQueue
     */
    std::recursive_mutex m_JobQueueMutex;

    /**
     * futures created by std::async block in their destructors
     * that's why they are kept alive in this list
     */
    std::list<std::shared_future<void>> m_AsyncJobs;
    std::mutex m_AsyncJobsMutex;
};

// need inline keyword to avoid multiple definitions at link time, when including this file in multiple compilation units
template <class ReturnType>
inline JobType<void> JobManager::wrapJob(JobType<ReturnType> job, std::shared_ptr<std::promise<ReturnType>> promise)
{
    return [promise, job](Engine::BaseEngine* engine) {
        promise->set_value(job(engine));
    };
}

// template specialization needed, because of promise<void>::set_value() specialization
template <>
inline JobType<void> JobManager::wrapJob(JobType<void> job, std::shared_ptr<std::promise<void>> promise)
{
    return [promise, job](Engine::BaseEngine* engine) {
        job(engine);
        promise->set_value();
    };
}

template <class ReturnType>
inline std::future<ReturnType> JobManager::executeInThread(JobType<ReturnType> job, ExecutionPolicy policy)
{
    if (!m_EnableMultiThreading && policy == ExecutionPolicy::NewThread)
        policy = ExecutionPolicy::MainThread;

    std::shared_ptr<std::promise<ReturnType>> promise = std::make_shared<std::promise<ReturnType>>();
    std::future<ReturnType> waitableFuture = promise->get_future();

    auto wrappedJob = wrapJob(std::move(job), promise);

    switch (policy)
    {
        case ExecutionPolicy::MainThread:
            if (isSameThread())
                wrappedJob(m_pEngine); // if caller is main thread, execute right away
            else
                queueJob(std::move(wrappedJob));
            break;
        case ExecutionPolicy::NewThread:
        {
            std::future<void> keepAliveFuture = std::async(std::launch::async, std::move(wrappedJob), m_pEngine);
            std::lock_guard<std::mutex> guard(m_AsyncJobsMutex);
            m_AsyncJobs.emplace_back(std::move(keepAliveFuture));
        }
            break;
    }
    return waitableFuture;
}
}