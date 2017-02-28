#include "GameClock.h"
#include <cmath>

using namespace Engine;

GameClock::GameClock()
{
    m_ClockSpeedFactor = 1.0;
    // day/clock init only necessary for test start world (not started via main menu)
    m_Day = 1;
    setTimeOfDay(8, 0);
}

int GameClock::getDay() const
{
    return m_Day;
}

void GameClock::setDay(int newDay) {
    m_Day = newDay;
}

void GameClock::update(float deltaRealTimeSeconds)
{
    m_TimeOfDay += totalSpeedUp() * deltaRealTimeSeconds / SECONDS_IN_A_DAY;
    if (m_TimeOfDay >= 1.0f)
    {
        float overFlowTimeOfDay = m_TimeOfDay;
        m_TimeOfDay = std::fmod(m_TimeOfDay, 1.0f);
        // handles also case where the ingame clock is updated by huge dt (> 1 day)
        m_Day += std::lround(overFlowTimeOfDay - m_TimeOfDay);
    }
}

void GameClock::getTimeOfDay(int& hours, int& minutes) const
{
    dayTimeTohm(m_TimeOfDay, hours, minutes);
}

float GameClock::getTimeOfDay() const
{
    return m_TimeOfDay;
}

std::string GameClock::getDateTimeFormatted() const
{
    return "Day " + std::to_string(m_Day) + ", " + getTimeOfDayFormatted();
}

void GameClock::setTimeOfDay(int hours, int minutes, bool onlyForward)
{
    float newTimeOfDay = hmToDayTime(hours, minutes);
    if (onlyForward && newTimeOfDay < m_TimeOfDay)
    {
        m_Day++;
    }
    m_TimeOfDay = newTimeOfDay;
}

void GameClock::setTotalSeconds(std::size_t s)
{
    float inDays = s / static_cast<float>(SECONDS_IN_A_DAY);
    m_Day = static_cast<int>(inDays);
    m_TimeOfDay = inDays - m_Day;
}

std::size_t GameClock::getTotalSeconds()
{
    return static_cast<std::size_t>((m_Day + m_TimeOfDay) * SECONDS_IN_A_DAY);
}

void GameClock::setClockSpeedFactor(float factor){
    m_ClockSpeedFactor = factor;
}

float GameClock::totalSpeedUp() const
{
    return GAMETIME_REALTIME_RATIO * m_ClockSpeedFactor;
}

float GameClock::getTime(){
    return m_Day + m_TimeOfDay;
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