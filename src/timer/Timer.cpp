#include "Timer.h"

Timer::Timer()
{

}

void Timer::start()
{
    if (!m_started) {
        m_started = true;
        m_startTime = SDL_GetTicks();
    }
}

void Timer::pause()
{
    if (!m_paused) {
        m_paused = true;
        m_pauseStartTime = SDL_GetTicks();
    }
}

void Timer::resume()
{
    if (m_paused) {
        Uint32 elapsedPaused = SDL_GetTicks() - m_pauseStartTime;
        m_startTime += elapsedPaused;
        m_paused = false;
    }
}

void Timer::stop()
{
    if (m_started)
        m_started = false;
}

float Timer::getSeconds() const
{
    float elapsed = getMillis();
    if (elapsed <= 0.0f)
        return elapsed;
    else
        return getMillis() / 1000.0f;
}

float Timer::getMillis() const
{
    if (!m_started)
        return -1.0f;

    if (m_paused)
        return static_cast<float>(SDL_GetTicks() - (m_startTime + SDL_GetTicks() - m_pauseStartTime));

    return static_cast<float>(SDL_GetTicks() - m_startTime);
}

Timer::~Timer()
{
    //dtor
}
