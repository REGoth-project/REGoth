#include "GameClock.h"
#include <cmath>

using namespace Engine;

GameClock::GameClock()
{
    lastFrameDeltaTime = 0.0;
    // day/clock init only necessary for test start world (not started via main menu)
    day = 1;
    setTimeOfDay(8, 0);

    // for test purpose make the clock run 7 times faster than usual gameplay
    setClockSpeedFactor(7);
}

int GameClock::getDay() const
{
    return day;
}

void GameClock::setDay(int newDay) {
    day = newDay;
}

void GameClock::update(float deltaRealTimeSeconds)
{
    timeOfDay += totalSpeedUp() * deltaRealTimeSeconds / SECONDS_IN_A_DAY;
    if (timeOfDay >= 1.0f)
    {
        float overFlowTimeOfDay = timeOfDay;
        timeOfDay = std::fmod(timeOfDay, 1.0f);
        // handles also case where the ingame clock is updated by huge dt (> 1 day)
        day += std::lround(overFlowTimeOfDay - timeOfDay);
    }
}

void GameClock::getTimeOfDay(int& hours, int& minutes) const
{
    dayTimeTohm(timeOfDay, hours, minutes);
}

float GameClock::getTimeOfDay() const
{
    return timeOfDay;
}

std::string GameClock::getDateTimeFormatted() const
{
    return "Day " + std::to_string(day) + ", " + getTimeOfDayFormatted();
}

void GameClock::setTimeOfDay(int hours, int minutes, bool onlyForward)
{
    float newTimeOfDay = hmToDayTime(hours, minutes);
    if (onlyForward && newTimeOfDay < timeOfDay)
    {
        day++;
    }
    timeOfDay = newTimeOfDay;
}

void GameClock::setTotalSeconds(std::size_t s)
{
    float inDays = s / static_cast<float>(SECONDS_IN_A_DAY);
    day = static_cast<int>(inDays);
    timeOfDay = inDays - day;
}

std::size_t GameClock::getTotalSeconds()
{
    return static_cast<std::size_t>((day + timeOfDay) * SECONDS_IN_A_DAY);
}

void GameClock::setClockSpeedFactor(float factor){
    clockSpeedFactor = factor;
}

float GameClock::totalSpeedUp() const
{
    return gameTimeRealTimeRatio * clockSpeedFactor;
}

float GameClock::getTime(){
    return day + timeOfDay;
}

std::string GameClock::getTimeOfDayFormatted() const
{
    int h, m;
    getTimeOfDay(h, m);
    return std::to_string(h) + ":" + (m < 10 ? "0" : "") + std::to_string(m);
}

float GameClock::hmToDayTime(int hours, int minutes)
{
    return (hours + minutes / 60.0f) / 24.0f;
}

void GameClock::dayTimeTohm(float timeOfDay, int& hours, int& minutes)
{
    hours = static_cast<int>(timeOfDay * 24);
    minutes = static_cast<int>((timeOfDay * 24 - hours) * 60);
}