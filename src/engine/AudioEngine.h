#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

typedef struct ALCdevice_struct ALCdevice;

namespace Engine
{

    class AudioWorld;

    /** The AudioEngine creates and manages AudioWorlds.
      *
      * The engine class matches the OpenAL device level. Each world has it's own AudioWorld which
      * represents an OpenAL context.
      *
      */
    class AudioEngine final
    {
    public:

        /** Initializes the AudioEngine.
         *
         * The @p device param specifies the device to use and can be determined
         * using enumerateDevices().
         *
         * @param device_name The name of the device to use.
         *
         * @see AudioEngine::enumerateDevices()
         *
         */
        AudioEngine(const std::string &device_name=std::string());

        /** Deinitializes the AudioEngine.
         */
        ~AudioEngine();

        /** Returns the OpenAL device used by this engine.
         *
         * @return The OpenAL device or nullptr when initialization has failed.
         */
        ALCdevice *getDevice() const { return m_Device; }

        /** Enumerates the audio devices available on the current machine.
         *
         * Note that not all OpenAL implementations support enumeration. In this case you'll
         * retrieve a single element list with an empty device name, indicating that it is
         * the default device.
         *
         * @param[out] devices A list of available devices.
         *
         */
        static void enumerateDevices(std::vector<std::string> &devices);

        /** Returns a text representation of an error code.
         *
         * @param The error code returned by alGetError() or alcGetError().
         *
         * @return The error text.
         *
         */
        static const char *getErrorString(size_t errorCode);

    private:

        ALCdevice *m_Device = nullptr;
    };

}
