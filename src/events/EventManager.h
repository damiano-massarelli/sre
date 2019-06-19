#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H
#include <map>
#include <vector>
#include <SDL.h>
#include <memory>
#include "EventListener.h"
#include "EventListenerCrumb.h"

class EventManager
{
    friend class Engine;

    private:
        /** m_event2listeners cannot be modified while iterating it. Two other support
          * maps are used to temporarily store values to add and remove */
        std::map<SDL_EventType, std::vector<EventListener*>> m_event2listeners;
        std::map<SDL_EventType, std::vector<EventListener*>> m_toAdd;
        std::map<SDL_EventType, std::vector<EventListener*>> m_toRemove;

        EventManager();

        void pushEnterFrameEvent(float* deltaMillis) const;

    public:
        static const SDL_EventType ENTER_FRAME_EVENT;

        EventManager(const EventManager& em) = delete;
        EventManager& operator=(const EventManager& em) = delete;

        CrumbPtr addListenerFor(SDL_EventType event, EventListener* listener, bool wantCrumb = false);

        /**
          * Same as addListenerFor but adds the listener to an existing crumb
          */
        void addListenerFor(SDL_EventType event, EventListenerCrumb* existingCrumb);

        void removeListenerFor(SDL_EventType event, EventListener* listener);

        /**
          * Dispatches all the events from the event queue
          */
        void dispatchEvents();

        /**
          * Dispatches a certain event to all the listeners registered for that event
          */
        void dispatchToListeners(SDL_Event& event);

        virtual ~EventManager();
};

#endif // EVENTMANAGER_H
