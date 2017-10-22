//
// Created by markus on 28.05.17.
//
#pragma once
#include <functional>
#include <future>
#include <list>
#include "BaseEngine.h"

namespace Engine
{
    class BaseEngine;

    template <class ReturnType>
    class SafeFuture
    {
    public:
        ReturnType wait()
        {
            return m_Future.get();
        }
        std::shared_future<ReturnType> m_Future;
    };

    namespace AsyncAction
    {


        /**
         * Executes the job in the specified thread
         * @tparam job job to execute
         * @param job the job to be executed
         * @param executionPolicy defines which thread should execute the job
         * @return SafeFuture on which any thread may call wait() or get()
         */

        template <class ReturnType>
        using JobType = std::function<ReturnType(BaseEngine* engine)>;

        static SafeFuture<void> executeInThread(const JobType<void>& job, BaseEngine* engine, ExecutionPolicy policy)
        {
            using ReturnType = void;

            bool disableMultithreading = false;
            if (disableMultithreading && policy == ExecutionPolicy::NewThread)
                policy = ExecutionPolicy::MainThreadQueued;

            std::shared_ptr<std::promise<ReturnType>> promise = std::make_shared<std::promise<ReturnType>>();
            std::shared_future<ReturnType> resultFuture = promise->get_future();

            auto wrapperJob = [job = std::move(job), promise, engine](BaseEngine* engine) mutable {
                job(engine);
                promise->set_value();
                // no need to set_exception of promise.
                // MainThread and MainThreadQueued will be invoked directly and thrown in main thread
                // std::async (NewThread) will set_exception automatically and processMessageQueue will rethrow with .get()
            };

            switch (policy)
            {
                case ExecutionPolicy::MainThread:
                    engine->executeInMainThread(std::move(wrapperJob), false);
                    break;
                case ExecutionPolicy::MainThreadQueued:
                    engine->executeInMainThread(std::move(wrapperJob), true);
                    break;
                case ExecutionPolicy::NewThread:
                {
                    std::future<ReturnType> keepAliveFuture = std::async(std::launch::async, std::move(wrapperJob), engine);
                    // TODO lock m_AsyncJobsMutex
                    engine->m_AsyncJobs.push_back(std::move(keepAliveFuture));
                }
                    break;
            }
            return SafeFuture<ReturnType>{resultFuture};
        }
    }
}