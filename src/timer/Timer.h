#ifndef TIMER_H
#define TIMER_H
#include <cstdint>
#include <SDL.h>

/**
  * A simple timer class to measure time intervals
  */
class Timer
{
private:
    bool mStarted = false;
    bool mPaused = false;

    Uint32 mStartTime = 0;
    Uint32 mPauseStartTime = 0;

public:
    Timer();

    /**
     * Starts the timer.
     * Every timer should be started before using it.
	 * If the timer is paused, the timer is also resumed. */
    void start();

	/**
	 * @return whether the timer is started (true even if paused)
	 */
	bool isStarted();

    /**
     * Pauses the timer */
    void pause();

	/**
	 * @return whether the  timer is paused (true even if not started) 
	 */
	bool isPaused();

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
	 * Returns the elapsed time in seconds from start
	 * Pauses are taken into account.
	 * @see start
	 * @return elapsed time in seconds, -1.0f if the timer is stopped */
    float getSeconds() const;

    /**
	 * Returns the elapsed time in milliseconds from start
	 * Pauses are taken into account.
	 * @see start
	 * @return elapsed time in milliseconds, -1.0f if the timer is stopped */
    float getMillis() const;
};

#endif // TIMER_H
