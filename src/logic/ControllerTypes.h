#pragma once

namespace Logic
{
    const int JSON_DUMP_INDENT = 4;

    /**
     * Base controller kinds
     */
    enum class EControllerType
    {
        Controller,
        PlayerController,
        VisualController,
        CameraController,
        ItemController,
        MobController,
        SoundController,
        MusicController,
    };
}