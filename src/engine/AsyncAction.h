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

    struct AsyncAction
    {
    public:
        template <class T>
        using JobType = std::function<T(BaseEngine* engine)>;

        template <class Callable>
        using ReturnType = decltype(std::declval<Callable>()(std::declval<BaseEngine*>()));

        /**
         * Like BaseEngine::onMessage, but returns a shared_future to wait for and accepts also non-copyable callables
         * Caution when calling wait() or get() on the shared_future when using policy=std::launch::deferred.
         * If the job wasn't started yet, wait() or get() causes execution in the caller thread.
         * Use wait_for/wait_until instead.
         * @tparam Callable any Callable type that accepts BaseEngine pointer. i.e. std::function<T(BaseEngine* engine)>
         * @param job may even be non-copyable (i.e. lambda capturing a non copyable type)
         * @param engine
         * @param policy if std::launch::deferred is passed, the job is scheduled on the main thread.
         * @return
         */
        template <class Callable>
        static std::shared_future<ReturnType<Callable>>
        onMessage(Callable job, BaseEngine* engine, std::launch policy = std::launch::deferred)
        {
            std::shared_future<AsyncAction::ReturnType<Callable>> future = std::async(policy, std::forward<Callable>(job), engine);
            std::function<bool(BaseEngine* engine)> waitJob = [future, policy](BaseEngine* engine) {
                switch (policy)
                {
                    case std::launch::deferred:
                        future.wait();
                        return true;
                    case std::launch::async:
                        return future.wait_for(std::chrono::nanoseconds(0)) == std::future_status::ready;
                }
                return true;
            };
            engine->onMessage(waitJob);
            return future;
        }

        bool run(BaseEngine& engine);

        JobType<bool> m_Job;
    };
}