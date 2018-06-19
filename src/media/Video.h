//
// Created by mplucinski on 06.04.18.
//

#pragma once
#include <memory>
#include <string>
#include <render/ViewList.h>

namespace Engine {
    class BaseEngine;
}

namespace Media {
    class Video;
    class VideoPlayer
    {
    public:
        VideoPlayer(Engine::BaseEngine &engine);
        ~VideoPlayer();
        void play(const std::string &fileName);
        void frameUpdate(double dt, uint16_t width, uint16_t height);
        bool active();

    private:
        Engine::BaseEngine &engine;
        std::shared_ptr<Video> currentVideo;
    };
}
