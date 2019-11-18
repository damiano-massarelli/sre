#include "events/EventManager.h"
#include <algorithm>

// Static variable definition
const SDL_EventType EventManager::ENTER_FRAME_EVENT = static_cast<SDL_EventType>(SDL_RegisterEvents(1));
const SDL_EventType EventManager::EXIT_FRAME_EVENT = static_cast<SDL_EventType>(SDL_RegisterEvents(1));
const SDL_EventType EventManager::PRE_RENDER_EVENT = static_cast<SDL_EventType>(SDL_RegisterEvents(1));

EventManager::EventManager()
{
}

CrumbPtr EventManager::addListenerFor(SDL_EventType event, EventListener* listener, bool wantCrumb)
{
    if (m_toAdd.count(event))
        m_toAdd.at(event).push_back(listener);
    else
        m_toAdd.insert(std::make_pair(event, std::vector<EventListener*>{listener}));

    std::unique_ptr<EventListenerCrumb> crumb;
    if (wantCrumb)
        crumb = std::make_unique<EventListenerCrumb>(event, listener);

    return crumb;
}

void EventManager::addListenerFor(SDL_EventType event, EventListenerCrumb* existingCrumb)
{
    existingCrumb->addEvent(event);
    addListenerFor(event, existingCrumb->getListener(), false);
}

void EventManager::removeListenerFor(SDL_EventType event, EventListener* listener)
{
    if (m_toRemove.count(event))
        m_toRemove.at(event).push_back(listener);
    else
        m_toRemove.insert(std::make_pair(event, std::vector<EventListener*>{listener}));
}

void EventManager::dispatchEvents()
{
    /* Dispatches all events */
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        dispatchToListeners(event);
    }

    /* Adds the listeners */
    for (auto it = m_toAdd.begin(); it != m_toAdd.end(); ++it)
        if (m_event2listeners.count(it->first)) // already has some listeners for this event, append
            m_event2listeners.at(it->first).insert(m_event2listeners.at(it->first).end(), it->second.begin(), it->second.end());
        else // No listener for this event, just copy
            m_event2listeners.insert(std::make_pair(it->first, it->second));

    m_toAdd.clear();
}

void EventManager::dispatchToListeners(SDL_Event& event)
{
    // Event type
    SDL_EventType eventType = static_cast<SDL_EventType>(event.type);
    if (m_event2listeners.count(eventType)) {
        // List of listeners for this type of event
        auto& listeners = m_event2listeners[eventType];
        for (auto it = listeners.begin(); it != listeners.end(); ) {
            /* Skip a listener if it is removed and removes it */
            if (m_toRemove.count(eventType)) {
                auto& removed = m_toRemove[eventType];
                const auto& listenerToRemove = std::find(removed.begin(), removed.end(), *it);
                if (listenerToRemove != removed.end()) {
                    removed.erase(listenerToRemove);
                    it = listeners.erase(it);
                    continue;
                }
            }

            (*it)->onEvent(event);
            ++it;
        }
    }
}


void EventManager::pushEnterFrameEvent(float* deltaMillis)
{
	pushEvent(ENTER_FRAME_EVENT, deltaMillis);
}

void EventManager::pushExitFrameEvent(float* deltaMillis)
{
	pushEvent(EXIT_FRAME_EVENT, deltaMillis);
}

void EventManager::pushPreRenderEvent(float* deltaMillis)
{
	pushEvent(PRE_RENDER_EVENT, deltaMillis);
}

void EventManager::pushEvent(SDL_EventType type, void* data1 /*= nullptr*/, void* data2 /*= nullptr*/)
{
	SDL_Event event;
	SDL_memset(&event, 0, sizeof(event));
	event.type = type;
	event.user.code = 0;
	event.user.data1 = data1;
	event.user.data2 = data2;
	dispatchToListeners(event);
}

EventManager::~EventManager()
{

}
