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
        void update(float deltaRealTimeSeconds);

        /**
         * Converts time to hours/minutes (24h format)
         * @param hours
         * @param minutes
         */
        void getTimeOfDay(int& hours, int& minutes) const;

        /**
         * @return time elapsed in days since last midnight (0:00)
         */
        float getTimeOfDay() const;

        /**
         * @return Day + time of day as string
         */
        std::string getDateTimeFormatted() const;

        /**
         * Set time to hours/minutes (24h format)
         * @param hours
         * @param minutes
         * @param onlyForward indicates whether the day should be incremented as well if given clock time is in past
         */
        void setTimeOfDay(int hours, int minutes, bool onlyForward=false);

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
         * sets the extra speed factor for test purposes
         */
        void setClockSpeedFactor(float factor);

        /**
         * gets the total ingame time to real time ratio
         */
        float totalSpeedUp() const;

        /**
         * @return time in days since "new game" started
         */
        float getTime();

        /**
         * @return time of day as string in hh:mm format
         */
        std::string getTimeOfDayFormatted() const;

        /**
         * helper function for conversion
         * @param hours
         * @param minutes
         * @return converts timespan in hours/minutes to days (float)
         */
        static float hmToDayTime(int hours, int minutes);

        /**
         * helper function for conversion
         * @param hours
         * @param minutes
         * @return converts timeOfDay in days (float) to hours/minutes
         */
        static void dayTimeTohm(float timeOfDay, int& hours, int& minutes);

        static constexpr unsigned int SECONDS_IN_A_DAY = 24 * 60 * 60;

    private:
        // Last deltatime-value we have gotten here
        double lastFrameDeltaTime;

        // Time elapsed in the game since last 00:00 in days (interval [0,1[)
        // TODO export/import this value in json for savegames
        float timeOfDay;

        // Number of full days elapsed in the game since "start new gothic game"
        // TODO export/import this value in json for savegames
        int day;

        // Defines how much faster the ingame clock runs compared to the real time clock
        float gameTimeRealTimeRatio = 14.5;

        // define an extra speedup for the ingame clock for test purposes
        float clockSpeedFactor = 1.0;
    };
}