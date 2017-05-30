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
        std::function<bool(BaseEngine& engine)> job;
        // if this is set to true, the action shall block the queue (aka not isOverlay)
        bool blocking;
    };
}