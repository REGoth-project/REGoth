//
// Created by mplucinski on 06.04.18.
//

#include <iostream>

#include <engine/BaseEngine.h>
#include <utils/logger.h>
#include <utils/Utils.h>
#include <fstream>
#include "Video.h"
#include "ui/ImageView.h"

#ifdef RE_ENABLE_FFMPEG
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}

namespace {

struct deleteAVCodecContext
{
    void operator()(AVCodecContext *ptr)
    {
        if (ptr)
            avcodec_free_context(&ptr);
    }
};

struct deleteAVFormatContext
{
    void operator()(AVFormatContext *ptr)
    {
        if (ptr)
            avformat_close_input(&ptr);
    }
};

struct deleteAVFrame
{
    void operator()(AVFrame *ptr)
    {
        if (ptr)
            av_frame_free(&ptr);
    }
};

using AVCodecContextPtr = std::unique_ptr<AVCodecContext, deleteAVCodecContext>;
using AVFormatContextPtr = std::unique_ptr<AVFormatContext, deleteAVFormatContext>;
using AVFramePtr = std::unique_ptr<AVFrame, deleteAVFrame>;

}

namespace Media {
    class Video
    {
    public:
        Video(VideoPlayer &player, Engine::BaseEngine &engine, const std::string &fileName)
                : player{player}, engine{engine}, fileName{fileName}
        {
            av_register_all();

            view = new UI::ImageView(engine);
            view->setHidden(true);
            view->setRelativeSize(false);

            engine.getRootUIView().addChild(view);
        }

        bool init(const uint16_t width, const uint16_t height)
        {
            frame.reset(av_frame_alloc());
            if (!frame) {
                LogWarn() << "Could not allocate frame";
                return false;
            }

            av_init_packet(&packet);
            packet.data = nullptr;
            packet.size = 0;

            std::string filePath = "/_work/data/video/" + fileName;
            std::string fileFullPath = Utils::getCaseSensitivePath(filePath, engine.getEngineArgs().gameBaseDirectory);

            if (!Utils::fileExists(fileFullPath))
            {
                LogError() << "Failed to find the video file at: " << fileFullPath;
                return false;
            }

            AVFormatContext *formatContextPtr = nullptr;
            if (avformat_open_input(&formatContextPtr, fileFullPath.c_str(), nullptr, nullptr) < 0) {
                LogWarn() << "Could not open file: " << fileFullPath;
                return false;
            }
            formatContext.reset(formatContextPtr);

            if (avformat_find_stream_info(formatContext.get(), nullptr) < 0) {
                LogWarn() << "Could not find stream information";
                return false;
            }

            if (!openCodecContext(videoStreamIndex, videoCodecContext, AVMEDIA_TYPE_VIDEO)) {
                LogWarn() << "Could not open video context";
                return false;
            }

            if (!openCodecContext(audioStreamIndex, audioCodecContext, AVMEDIA_TYPE_AUDIO)) {
                LogWarn() << "Could not open audio context";
                return false;
            }

            av_dump_format(formatContext.get(), 0, fileFullPath.c_str(), 0); // TODO: dump to logger

            initialized_ = true;
            return true;
        }

    private:
        bool openCodecContext(int &streamIndex, AVCodecContextPtr &codecContext, AVMediaType mediaType)
        {
            int index = av_find_best_stream(formatContext.get(), mediaType, -1, -1, nullptr, 0);
            if (index < 0) {
                LogWarn() << "Could not find stream" << av_get_media_type_string(mediaType);
                return false;
            }

            AVStream *stream = formatContext->streams[index];

            AVCodec *codec = avcodec_find_decoder(stream->codecpar->codec_id);
            if (!codec) {
                LogWarn() << "Could not find codec for stream" << av_get_media_type_string(mediaType);
                return false;
            }

            AVCodecContextPtr context{avcodec_alloc_context3(codec)};
            if (!context) {
                LogWarn() << "Could not allocate codec context for stream" << av_get_media_type_string(mediaType);
                return false;
            }

            if (avcodec_parameters_to_context(context.get(), stream->codecpar) < 0) {
                LogWarn() << "Could not copy decoder parameters to decoder context for stream" << av_get_media_type_string(mediaType);
                return false;
            }

            AVDictionary *opts = nullptr;
            av_dict_set(&opts, "refcounted_frames", "0", 0);
            if (avcodec_open2(context.get(), codec, &opts) < 0) {
                LogWarn() << "Could not open codec for stream" << av_get_media_type_string(mediaType);
                return false;
            }

            streamIndex = index;
            std::swap(codecContext, context);
            return true;
        }

    public:
        void nextFrame()
        {
            int r = av_read_frame(formatContext.get(), &packet);
            if (r < 0) {// no more frames or error
                std::cout << "nextframe abort 1" << std::endl;
                ::abort();
            }

            if (packet.stream_index == videoStreamIndex) {
                r = avcodec_send_packet(videoCodecContext.get(), &packet);
                if(r < 0 || r == AVERROR(EAGAIN) || r == AVERROR_EOF) {
                    std::cout << "avcodec_send_packet: " << r << std::endl;
                    ::abort();
                }

                while (r >= 0) {
                    r = avcodec_receive_frame(videoCodecContext.get(), frame.get());
                    if (r == AVERROR(EAGAIN) || r == AVERROR_EOF) {
                        std::cout << "avcodec_receive_frame: " << r << std::endl;
                        break;
                    }
                    std::cout << "video frame: " << videoCodecContext->frame_number << std::endl;

                    view->setHidden(false);
                    view->setSize(Math::float2(1,1));

                    if (videoCodecContext->pix_fmt != AV_PIX_FMT_YUV420P) {
                        LogWarn() << "Unknown pixel format";
                        return;
                    }

                    Textures::TextureAllocator& alloc = engine.getEngineTextureAlloc();

                    std::vector<uint8_t> data;
                    int count = videoCodecContext->width * videoCodecContext->height;
                    auto clamp = [](int x){ return std::min(255, std::max(0, x)); };

                    // conversion from YUV420p to RGBA
                    // TODO: this can be probably accelerated by doing conversion within the fragment shader

                    uint8_t *src = frame->data[0];
                    int linesize = frame->linesize[0];
                    int w = videoCodecContext->width;
                    int h = videoCodecContext->height;

                    uint8_t *dataY = frame->data[0];
                    int linesizeY = frame->linesize[0];

                    uint8_t *dataU = frame->data[1];
                    int linesizeU = frame->linesize[1];

                    uint8_t *dataV = frame->data[2];
                    int linesizeV = frame->linesize[2];

                    for (int y = 0; y < h; ++y)
                        for (int x = 0; x < w; ++x) {
                            int Y = dataY[y*linesizeY+x];
                            int U = dataU[y/2*linesizeU+x/2];
                            int V = dataV[y/2*linesizeV+x/2];

                            int C = Y - 16;
                            int D = U - 128;
                            int E = V - 128;

                            int R = clamp( (298*C         + 409*E + 128 ) >> 8 );
                            int G = clamp( (298*C - 100*D - 208*E + 128 ) >> 8 );
                            int B = clamp( (298*C + 516*D         + 128 ) >> 8 );

                            data.push_back(R);
                            data.push_back(G);
                            data.push_back(B);
                            data.push_back(255);
                        }

                    if (!texture.isValid())
                        texture = alloc.loadTextureRGBA8(data, videoCodecContext->width, videoCodecContext->height);
                    else
                        alloc.getTexture(texture).imageData = data;
                    alloc.asyncFinalizeLoad(texture);
                    view->setImage(texture, videoCodecContext->width, videoCodecContext->height);
                }
            } else if(packet.stream_index == audioStreamIndex) {
                std::cout << "An audio frame" << std::endl;
            } else {
                LogWarn() << "Invalid stream index";
            }
        }

        bool initialized() const {
            return initialized_;
        }

        VideoPlayer &player;
        Engine::BaseEngine &engine;
        const std::string fileName;
        bool initialized_ = false;
        AVFormatContextPtr formatContext;
        int videoStreamIndex, audioStreamIndex;
        AVCodecContextPtr videoCodecContext, audioCodecContext;
        AVFramePtr frame;
        AVPacket packet;
        Handle::TextureHandle texture;
        UI::ImageView *view;
    };
}
#endif

Media::VideoPlayer::VideoPlayer(Engine::BaseEngine &engine): engine{engine}
{
}

Media::VideoPlayer::~VideoPlayer() {}

void Media::VideoPlayer::play(const std::string &fileName)
{
    std::cout << "playVideo(" << fileName << ")" << std::endl;
#ifdef RE_ENABLE_FFMPEG
    currentVideo = std::make_unique<Video>(*this, engine, fileName);
#else
    LogWarn() << "No libavcodec support compiled, won't play" << fileName;
#endif
}

void Media::VideoPlayer::frameUpdate(double dt, uint16_t width, uint16_t height)
{
#ifdef RE_ENABLE_FFMPEG
    if (!currentVideo->initialized())
        if (!currentVideo->init(width, height)) {
            LogWarn() << "Could not initialize video player";
            currentVideo = nullptr;
            return;
        }

    currentVideo->nextFrame();
#endif
}

bool Media::VideoPlayer::active() {
#ifdef RE_ENABLE_FFMPEG
    return currentVideo != nullptr;
#else
    return false;
#endif
}
