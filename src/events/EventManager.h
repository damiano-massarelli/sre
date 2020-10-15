#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H
#include "events/EventListener.h"
#include "events/EventListenerCrumb.h"
#include <SDL.h>
#include <map>
#include <memory>
#include <vector>

class EventManager {
    friend class Engine;

private:
    /** m_event2listeners cannot be modified while iterating it. Two other
     * support maps are used to temporarily store values to add and remove */
    std::map<SDL_EventType, std::vector<EventListener*>> m_event2listeners;
    std::map<SDL_EventType, std::vector<EventListener*>> m_toAdd;
    std::map<SDL_EventType, std::vector<EventListener*>> m_toRemove;

    EventManager();

    void pushEnterFrameEvent(float* deltaMillis);

    void pushExitFrameEvent(float* deltaMillis);

    void pushPreRenderEvent(float* deltaMillis);

    void pushEvent(SDL_EventType type, void* data1 = nullptr, void* data2 = nullptr);

public:
    /** Enter frame event. Emitted every time a new frame begins */
    static const SDL_EventType ENTER_FRAME_EVENT;

    /** Exit frame event. Emitted every time a frame ends */
    static const SDL_EventType EXIT_FRAME_EVENT;

    /** Emitted every frame just before rendering begins: after enter frame
     * events and before exit frame */
    static const SDL_EventType PRE_RENDER_EVENT;

    /** Receives all events */
    static const SDL_EventType ALL_EVENTS;

    EventManager(const EventManager& em) = delete;
    EventManager& operator=(const EventManager& em) = delete;

    CrumbPtr addListenerFor(SDL_EventType event, EventListener* listener, bool wantCrumb = false);

    /**
     * Same as addListenerFor but adds the listener to an existing crumb
     */
    void addListenerFor(SDL_EventType event, EventListenerCrumb* existingCrumb);

    /**
     * Removes a listener for a given event.
     * @param event the listener will no more receive notifications for this
     * event.
     * @listener the listener
     */
    void removeListenerFor(SDL_EventType event, EventListener* listener);

    /**
     * Dispatches all the events from the event queue
     */
    void dispatchEvents();

    /**
     * Dispatches a certain event to all the listeners registered for that event
     */
    void dispatchToListeners(SDL_EventType eventType, SDL_Event& event);

    virtual ~EventManager();
};

#endif  // EVENTMANAGER_H
