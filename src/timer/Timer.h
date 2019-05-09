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
        bool m_started = false;
        bool m_paused = false;

        Uint32 m_startTime = 0;
        Uint32 m_pauseStartTime = 0;

    public:
        Timer();

        /**
          * Starts the timer.
          * Every timer should be started before using it */
        void start();

        /**
          * Pauses the timer */
        void pause();

        /**
          * Resumes the timer */
        void resume();

        /**
          * Stops the time */
        void stop();

        /**
          * Returns the elapsed time in seconds from start
          * Pauses are taken into account.
          * @see start
          * @return elapsed time in seconds, -1.0f if the timer is stopped
          */
        float getSeconds() const;

        /**
          * Returns the elapsed time in milliseconds from start
          * Pauses are taken into account.
          * @see start
          * @return elapsed time in milliseconds, -1.0f if the timer is stopped
          */
        float getMillis() const;

        virtual ~Timer();
};

#endif // TIMER_H
