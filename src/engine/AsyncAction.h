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

    struct AsyncAction
    {
    public:
        template <class T>
        using JobType = std::function<T(BaseEngine* engine)>;

        bool run(BaseEngine& engine);

        JobType<bool> m_Job;
    };
}