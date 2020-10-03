#include "timer/Timer.h"

void Timer::start()
{
    if (!mStarted) {
		mPaused = false;
        mStarted = true;
        mStartTime = SDL_GetTicks();
        mSnapshot = 0;
    }
}

bool Timer::isStarted() const
{
	return mStarted;
}

void Timer::pause()
{
    if (mStarted && !mPaused) {
        mSnapshot = static_cast<Uint32>(getMillis());
        mPaused = true;
    }
}

bool Timer::isPaused() const
{
	return mPaused;
}

void Timer::resume()
{
    if (mPaused) {
        mStartTime = SDL_GetTicks();
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

void Timer::setTimeSpeedMultiplier(const float multiplier)
{
    mSnapshot = static_cast<Uint32>(getMillis());
    mStartTime = SDL_GetTicks();
    mTimeSpeedMultiplier = multiplier;
}

float Timer::getTimeSpeedMultiplier() const
{
    return mTimeSpeedMultiplier;
}

float Timer::getSeconds() const
{
    return getMillis() / 1000.0f;
}

void Timer::setSeconds(float seconds)
{
    if (mStarted) {
        mSnapshot = static_cast<Uint32>(seconds * 1000);
        mStartTime = SDL_GetTicks();
    }
}

float Timer::getMillis() const
{
    if (!mStarted)
        return 0.0f;

    if (mPaused)
        return static_cast<float>(mSnapshot);

    return static_cast<float>(mSnapshot + (SDL_GetTicks() - mStartTime) * mTimeSpeedMultiplier);
}
