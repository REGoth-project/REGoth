#include "GameClock.h"
#include <cmath>
#include <math/mathlib.h>
#include <ZenLib/utils/logger.h>

using namespace Engine;

GameClock::GameClock()
{
    m_totalTimeInDays = 0;
    m_LastFrameDeltaTime = 0;

    // reset clock speed and game speed to default value on new session
    m_ClockSpeedFactor = 1.0;
    m_GameEngineSpeedFactor = 1.0;

    // for test purpose make the clock run 7 times faster than usual gameplay
    // setClockSpeedFactor(7.0);

    resetNewGame();
}

int GameClock::getDay() const
{
    return lround(floor(m_totalTimeInDays));
}

void GameClock::setDay(int newDay) {
    m_totalTimeInDays += newDay - getDay();
}

void GameClock::update(double deltaRealTimeSeconds)
{
    m_LastFrameDeltaTime = deltaRealTimeSeconds;
    m_totalTimeInDays += totalSpeedUp() * deltaRealTimeSeconds / SECONDS_IN_A_DAY;
}

void GameClock::getTimeOfDay(int& hours, int& minutes) const
{
    dayTimeTohm(getTimeOfDay(), hours, minutes);
}

double GameClock::getTimeOfDay() const
{
    return m_totalTimeInDays - getDay();
}

std::string GameClock::getDateTimeFormatted() const
{
    return "Day " + std::to_string(getDay()) + ", " + getTimeOfDayFormatted();
}

void GameClock::setTimeOfDay(int hours, int minutes)
{
    m_totalTimeInDays = getDay() + hmToDayTime(hours, minutes);
}

void GameClock::setTotalSeconds(std::size_t s)
{
    m_totalTimeInDays = s / static_cast<double>(SECONDS_IN_A_DAY);
}

std::size_t GameClock::getTotalSeconds()
{
    return static_cast<std::size_t>(m_totalTimeInDays * SECONDS_IN_A_DAY);
}

void GameClock::setClockSpeedFactor(float factor){
    m_ClockSpeedFactor = factor;
}

float GameClock::totalSpeedUp() const
{
    return GAMETIME_REALTIME_RATIO * m_ClockSpeedFactor;
}

double GameClock::getTime(){
    return m_totalTimeInDays;
}

std::string GameClock::getTimeOfDayFormatted() const
{
    int h, m;
    getTimeOfDay(h, m);
    return std::to_string(h) + ":" + (m < 10 ? "0" : "") + std::to_string(m);
}

double GameClock::hmToDayTime(int hours, int minutes)
{
    return (hours + minutes / 60.0f) / 24.0f;
}

void GameClock::dayTimeTohm(double timeOfDay, int& hours, int& minutes)
{
    hours = static_cast<int>(timeOfDay * 24);
    minutes = static_cast<int>((timeOfDay * 24 - hours) * 60);
}

void GameClock::resetNewGame() {
    m_totalTimeInDays = 0;
    setDay(0);
    setTimeOfDay(8, 0);
}

double GameClock::getLastDt() {
    return m_LastFrameDeltaTime;
}
