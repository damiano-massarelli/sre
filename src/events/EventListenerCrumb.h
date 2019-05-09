#ifndef EVENTLISTENERCRUMB_H
#define EVENTLISTENERCRUMB_H
#include "EventListener.h"
#include <SDL.h>
#include <vector>

/**
  * Event listener crumbs automatically unregister an event listener when it is deleted.
  * The EventListener receiving
  * it should store it in a member variable so that when the EventListener is destroyed the EventListenerCrumb
  * can automatically unregister it.
  */
class EventListenerCrumb
{
    private:
        std::vector<SDL_EventType> m_eventTypes;
        EventListener* m_listener;

    public:
        EventListenerCrumb();

        EventListenerCrumb(SDL_EventType eventType, EventListener* listener);

        /**
          * Unregister the EventListener attached to this crumb.
          */
        void unregister();

        /**
          * Returns the listener attached to this event crumb */
        EventListener* getListener();

        /**
          * Adds another event to this event crumb.
          * This is used when an event listener is already attached to an event
          * crumb for an event X and it has to be attached to event Y using the same crumb
          * @param eventType the type of the event to be attached to
          */
        void addEvent(SDL_EventType eventType);

        virtual ~EventListenerCrumb();

};

#endif // EVENTLISTENERCRUMB_H
