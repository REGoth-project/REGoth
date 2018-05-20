//
// Created by andre on 20.05.18.
//

#include "VideoView.h"
#include <FfmpegVideoPlayer/VideoRead.h>
#include <ZenLib/utils/logger.h>
#include <render/RenderSystem.h>

UI::VideoView::VideoView(Engine::BaseEngine& e) : View(e)
{

}

UI::VideoView::~VideoView()
{

}

void UI::VideoView::update(double dt, Engine::Input::MouseState& mstate, Render::RenderConfig& config)
{
    View::update(dt, mstate, config);

    if(!m_QueuedVideoFile.empty())
    {
        playVideo(m_QueuedVideoFile, config); // Blocks until done!

        m_QueuedVideoFile.clear();
    }
}

void UI::VideoView::playVideo(const std::string& file, Render::RenderConfig& config)
{
    bgfx::TextureHandle videoTexture;
    VideoRead::VideoInfo videoInfo;

    VideoRead::PlayerSettings playerSettings = {
            .onVideoInit = [&](const VideoRead::VideoInfo& info) {
                LogInfo() << "Video resolution: " << info.width << "x" << info.height;

                videoInfo = info;
                videoTexture = bgfx::createTexture2D(info.width, info.height, false, 1, bgfx::TextureFormat::RGB8);
            },

            .onVideoFrame = [&](const VideoRead::VideoFrameData& data) {
                LogInfo() << "New video frame ready!";

                if(!bgfx::isValid(videoTexture))
                    throw new std::runtime_error("Failed to create/write to video-texture");

                bgfx::updateTexture2D(videoTexture,
                                      0, 0, 0, 0,
                                      (uint16_t)videoInfo.width, (uint16_t)videoInfo.height,
                                      bgfx::copy(data.rgb.data(), (uint32_t)data.rgb.size()));

                drawTexture(BGFX_VIEW, 0, 0,
                            config.state.viewWidth, config.state.viewHeight,
                            config.state.viewWidth, config.state.viewHeight,
                            videoTexture,
                            config.programs.imageProgram,
                            config.uniforms.diffuseTexture);

                bgfx::frame();
            },
    };

    try
    {
        VideoRead::playVideo(file, playerSettings);
    }
    catch(std::runtime_error& err)
    {
        LogError() << "Failed to play video. Reason: " << err.what();
    }
}

void UI::VideoView::enqueueVideo(const std::string& file)
{
    m_QueuedVideoFile = file;
}
