#ifndef TIMER_H
#define TIMER_H
#include <SDL.h>
#include <cstdint>

/**
 * A simple timer class to measure time intervals.
 */
class Timer {
private:
    bool mStarted = false;
    bool mPaused = false;

    Uint32 mStartTime = 0;
    Uint32 mSnapshot = 0;

    float mTimeSpeedMultiplier = 1.f;

public:
    Timer() = default;

    /**
     * Starts the timer.
     * Every timer should be started before using it.
     * If the timer is paused, the timer is also resumed. */
    void start();

    /**
     * @return whether the timer is started (true even if paused)
     */
    bool isStarted() const;

    /**
     * Pauses the timer */
    void pause();

    /**
     * @return whether the  timer is paused (true even if not started)
     */
    bool isPaused() const;

    /**
     * Resumes the timer */
    void resume();

    /**
     * Stops the time */
    void stop();

    /**
     * Resets the timer.
     * Same as calling stop() and then start()
     */
    void reset();

    /**
     * Sets a multiplier for the speed of time.
     * This value defaults to 1.f which means that time proceeds normally.
     * @param multiplier the time multiplier
     */
    void setTimeSpeedMultiplier(const float multiplier);

    /**
     * @return the time speed multiplier.
     */
    float getTimeSpeedMultiplier() const;

    /**
     * Returns the elapsed time in seconds from start
     * Pauses are taken into account.
     * @see start
     * @return elapsed time in seconds, 0.0f if the timer is stopped */
    float getSeconds() const;

    /**
     * Sets the elapsed time in seconds.
     * Only works if the timer is started or paused.
     * @param seconds the number of elapsed seconds
     */
    void setSeconds(float seconds);

    /**
     * Returns the elapsed time in milliseconds from start.
     * Pauses are taken into account.
     * @see start
     * @return elapsed time in milliseconds, 0.0f if the timer is stopped */
    float getMillis() const;
};

#endif  // TIMER_H
