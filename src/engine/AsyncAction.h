//
// Created by markus on 28.05.17.
//
#pragma once
#include <functional>
#include <list>
#include <future>

namespace Engine
{
    class BaseEngine;

    class AsyncAction
    {
    public:
        using JobType = std::function<bool(BaseEngine& engine)>;

        /**
         * move constructor. useful if the function (i.e. lambda) captures some huge object
         * @param job
         */
        AsyncAction(JobType&& job);
        AsyncAction(const JobType& job);

        /**
         * execute the job
         * @param engine
         * @return true if the job has finished and shall be removed from the queue
         */
        bool run(BaseEngine& engine);

    private:
        bool m_Finished;

        JobType m_Job;
    };
}