#include "Timer.h"

Timer::Timer()
{

}

void Timer::start()
{
    if (!mStarted) {
		mPaused = false;
        mStarted = true;
        mStartTime = SDL_GetTicks();
    }
}

bool Timer::isStarted()
{
	return mStarted;
}

void Timer::pause()
{
    if (!mPaused) {
        mPaused = true;
        mPauseStartTime = SDL_GetTicks();
    }
}

bool Timer::isPaused()
{
	return mPaused;
}

void Timer::resume()
{
    if (mPaused) {
        Uint32 elapsedPaused = SDL_GetTicks() - mPauseStartTime;
        mStartTime += elapsedPaused;
        mPaused = false;
    }
}

void Timer::stop()
{
    if (mStarted)
        mStarted = false;
}

void Timer::reset()
{
	stop();
	start();
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
    if (!mStarted)
        return 0.0f;

    if (mPaused)
        return static_cast<float>(SDL_GetTicks() - (mStartTime + SDL_GetTicks() - mPauseStartTime));

    return static_cast<float>(SDL_GetTicks() - mStartTime);
}
