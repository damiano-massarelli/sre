#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H
#include <SDL.h>

/** A simple interface for objects desiring to receive event notifications. */
class EventListener
{
public:
    virtual void onEvent(SDL_Event e) = 0;

    virtual ~EventListener(){};

};

#endif // EVENTLISTENER_H
