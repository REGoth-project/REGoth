#pragma once
#include <set>
#include "Controller.h"
#include "LogicDef.h"
#include <daedalus/DaedalusGameState.h>

namespace Logic
{
    class SoundController : public Controller
    {
    public:
        /**
         * @param world World of the underlaying entity
         * @param entity Entity owning this controller
         */
        SoundController(World::WorldInstance& world, Handle::EntityHandle entity);

        /**
         * Called on game-tick
         */
        void onUpdate(float deltaTime) override;

        /**
         * Initializes this controller with data loaded from a .ZEN-file
         * @param vob Data loaded from the ZEN
         */
        void initFromVobDescriptor(const ZenLoad::zCVobData& vob);

        /**
         * @return The type of this class. If you are adding a new base controller, be sure to add it to ControllerTypes.h
         */
        virtual EControllerType getControllerType() { return EControllerType::SoundController; }
        /**
         * Plays the given soundfile applying the settings of this controller
         */
        void playSound(const std::string& sound);

        /**
         * @return Classes which want to get exported on save should return true here
         */
        virtual bool shouldExport() { return false; }  // TODO: Implement saving/loading
        virtual void importObject(const json& j);

    protected:
        /**
         * @return True, if the current camera is close enough for the sound to be heard
         */
        bool isInHearingRange();

        /**
         * Handles setting when the sound should be played next, if it's using the random-delay mode
         */
        void setNextPlayingTimeRandomized();

        virtual void exportPart(json& j) override;

        /**
         * Name of the played sound
         */
        std::string m_SoundFile;

        /**
         * Delay between loops (In Seconds)
         */
        float m_SoundPlayDelay = 0.0f;

        /**
         * Maximum random value to be subtracted or added to the SoundPlayDelay (In Seconds)
         */
        float m_SoundDelayRandomness = 0.0f;

        /**
         * Maximum distance this sound can be heard from
         */
        float m_SoundMaxDistance = FLT_MAX;

        /**
         * Handle to the currently played or last played sound
         */
        Utils::Ticket<World::AudioWorld> m_PlayedSound;

        /**
         * Time when to play this for the next time (seconds)
         */
        float m_SoundTimePlayNextRandom = 0.0f;

        /**
         * Whether this sound should be looping/triggered/etc
         */
        ZenLoad::SoundMode m_SoundMode = ZenLoad::SoundMode::SM_ONCE;

        /**
         * How often this sound has been played
         */
        size_t m_NumTimesPlayed = 0;
    };
}
