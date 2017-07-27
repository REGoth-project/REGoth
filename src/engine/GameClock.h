#pragma once

#include <string>

namespace Engine
{
    /**
     * Ingame clock
     */
    class GameClock
    {
    public:
        GameClock();

        /**
         * @return the delta time of the last frame update
         */
        double getLastDt();

        /**
         * sets the clock to the default settings of a new game
         */
        void resetNewGame();

        /**
         * @return current day
         */
        int getDay() const;

        /**
        * @param newDay day to be set to
        */
        void setDay(int newDay);

        /**
         * updates the game time with the given real time
         */
        void update(double deltaRealTimeSeconds);

        /**
         * Converts time to hours/minutes (24h format)
         * @param hours
         * @param minutes
         */
        void getTimeOfDay(int& hours, int& minutes) const;

        /**
         * @return time elapsed in days since last midnight (0:00)
         */
        double getTimeOfDay() const;

        /**
         * @return Day + time of day as string
         */
        std::string getDateTimeFormatted() const;

        /**
         * Set time to hours/minutes (24h format)
         * hours + minutes / 60.0 may be greater than 24 or smaller than 0
         * the day will be adjusted in this case
         * @param hours
         * @param minutes
         */
        void setTimeOfDay(int hours, int minutes);

        /**
         * sets the total time directly
         * @param t time in seconds
         */
        void setTotalSeconds(std::size_t t);

        /**
         * gets the total time
         * @return time in seconds
         */
        std::size_t getTotalSeconds();

        /**
         * @return time in seconds since day 0 0:00
         */
        double getTotalSecondsFrac();

        /**
         * sets the extra speed factor for test purposes
         */
        void setClockSpeedFactor(float factor);

        /**
         * gets the total ingame time to real time ratio
         */
        float totalSpeedUp() const;

        /**
         * @return time in days since day 0 0:00
         */
        double getTime();

        /**
         * @return time of day as string in hh:mm format
         */
        std::string getTimeOfDayFormatted() const;

        /**
         * helper function for conversion
         * @param hours
         * @param minutes
         * @return converts timeOfDay in hours/minutes to days (double)
         */
        static double hmToDayTime(int hours, int minutes);

        /**
         * helper function for conversion
         * @param hours
         * @param minutes
         * @return converts timeOfDay in days (double) to hours/minutes
         */
        static void dayTimeTohm(double timeOfDay, int& hours, int& minutes);

        /**
         * additional factor for deltaTime when updating world instances. Speeds up everything
         * i.e. world (ergo animations), in game clock (ergo sky) even the camera
         * only for fun / debug purpose (default = 1.0)
         * @param factor
         */
        void setGameEngineSpeedFactor(float factor)
        {
            m_GameEngineSpeedFactor = factor;
        }

        /**
         * @return m_GameEngineSpeedFactor
         */
        float getGameEngineSpeedFactor() const
        {
            return m_GameEngineSpeedFactor;
        }

        /**
         * @return Seconds since program start
         */
        double getTotalSecondsRealtime() const
        {
            return m_TotalSecondsRunning;
        }

        static constexpr unsigned int SECONDS_IN_A_DAY = 24 * 60 * 60;

        // Defines how much faster the ingame clock runs compared to the real time clock. Don't change this value
        static constexpr float GAMETIME_REALTIME_RATIO = 14.5;

    private:
        // Time elapsed since Day 0 00:00 in days
        double m_totalTimeInDays;

        // define an extra speedup for the ingame clock
        float m_ClockSpeedFactor;

        // last known delta t
        double m_LastFrameDeltaTime;

        // Seconds since program start
        double m_TotalSecondsRunning;

        // Global speed factor for the engine
        float m_GameEngineSpeedFactor;
    };
}