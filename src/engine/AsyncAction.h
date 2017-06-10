//
// Created by markus on 28.05.17.
//
#pragma once
#include <functional>
#include <list>
#include <future>
#include "BaseEngine.h"

namespace Engine
{
    class BaseEngine;

    template <class T>
    class SafeFuture
    {
    public:
        SafeFuture(std::shared_future<T> f, BaseEngine* pBaseEngine) :
                m_pBaseEngine(pBaseEngine),
                m_SharedFuture(f)
        {}

        /**
         * Safely waits for the future to be started and finished
         * If the calling thread is the main thread, the execution will be triggered additionally
         * It is safe call this function from any thread
         * @return result of the future
         */
        const T& get()
        {
            wait();
            return m_SharedFuture.get();
        }

        /**
         * Safely waits for the future to be started and finished
         * If the calling thread is the main thread, the execution will be triggered additionally
         * It is safe call this function from any thread
         */
        void wait()
        {
            if (!m_pBaseEngine->isMainThread())
            {
                // wait until the main thread has started the execution
                // timespan doesn't matter, since it is ignored if the status is deferred
                while (m_SharedFuture.wait_for(std::chrono::seconds(0)) == std::future_status::deferred)
                {}
            }
            m_SharedFuture.wait();
        }

    private:
        BaseEngine* m_pBaseEngine;
        std::shared_future<T> m_SharedFuture;
    };

    struct AsyncAction
    {
    public:
        template <class T>
        using JobType = std::function<T(BaseEngine* engine)>;

        template <class Callable>
        using ReturnType = decltype(std::declval<Callable>()(std::declval<BaseEngine*>()));

        /**
         * Executes the job in the specified thread
         * @tparam Callable any callable type that accepts BaseEngine pointer. i.e. std::function<T(BaseEngine* engine)>
         * @param job a callable object. Unlike BaseEngine::onMessage also accepts non-copyable objects
         * @param engine
         * @param executionPolicy defines which thread should execute the job
         * @param forceQueueing if false and if called from main thread the job will be executed right away
         *        (only relevant for main thread -> main thread jobs)
         * @return SafeFuture on which any thread may call wait() or get()
         */
        template <class Callable>
        static std::shared_future<ReturnType<Callable>>
        executeInAnyThread(Callable job, BaseEngine *engine, ExecutionPolicy executionPolicy, bool forceQueueing = false)
        {
            std::launch policy;
            switch (executionPolicy)
            {
                case ExecutionPolicy::MainThread:
                    policy = std::launch::deferred;
                    break;
                case ExecutionPolicy::NewThread:
                    policy = std::launch::async;
                    break;
            }
            std::shared_future<AsyncAction::ReturnType<Callable>> future = std::async(policy, std::forward<Callable>(job), engine);
            std::function<bool(BaseEngine* engine)> wrapperJob = [future, policy](BaseEngine* engine) -> bool{
                switch (policy)
                {
                    case std::launch::deferred:
                        assert(engine->isMainThread());
                        future.wait();
                        return true;
                    case std::launch::async:
                        return future.wait_for(std::chrono::nanoseconds(0)) == std::future_status::ready;
                }
                return true;
            };
            // executeInMainThreadUntilTrue makes the job stay in the queue until the future is ready
            engine->executeInMainThreadUntilTrue(wrapperJob, forceQueueing);
            return future;
        }

        bool run(BaseEngine& engine);

        JobType<bool> m_Job;
    };
}