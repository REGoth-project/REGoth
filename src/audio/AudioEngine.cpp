#include <cstddef>
#include <cstring>

#ifdef RE_USE_SOUND
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include <utils/logger.h>

#include "AudioEngine.h"

namespace Audio
{

void AudioEngine::enumerateDevices(std::vector<std::string> &enumerated)
{
#ifdef RE_USE_SOUND
    if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT"))
    {
        size_t len = 0;
        const ALCchar *devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
        const ALCchar *device = devices, *next = devices + 1;

        while (device && *device != '\0' && next && *next != '\0')
        {
            enumerated.push_back(device);

            len = strlen(device);
            device += (len + 1);
            next += (len + 2);
        }
    }

    if (enumerated.empty())
        enumerated.push_back(std::string()); // empty string is default device
#endif
}

const char *AudioEngine::getErrorString(size_t errorCode)
{
#ifdef RE_USE_SOUND
    switch (errorCode)
    {
    case AL_NO_ERROR:
        return "AL_NO_ERROR";
    case AL_INVALID_NAME:
        return "AL_INVALID_NAME";
    case AL_INVALID_ENUM:
        return "AL_INVALID_ENUM";
    case AL_INVALID_VALUE:
        return "AL_INVALID_VALUE";
    case AL_INVALID_OPERATION:
        return "AL_INVALID_OPERATION";
    case AL_OUT_OF_MEMORY:
        return "AL_OUT_OF_MEMORY";
    }
    return "UNKNOWN";
#endif
}

AudioEngine::AudioEngine(const std::string &name)
{
#ifdef RE_USE_SOUND
    m_Device = alcOpenDevice(name.empty() ? NULL : name.c_str());
    if (!m_Device)
    {
        LogWarn() << "Could not open audio device '" << (name.empty() ? "default" : name) << "': "
                  << getErrorString(alGetError()) << ", sound disabled";
        return;
    }
#endif
}

AudioEngine::~AudioEngine()
{
#ifdef RE_USE_SOUND
    if (m_Device)
        alcCloseDevice(m_Device);
#endif
}

}
