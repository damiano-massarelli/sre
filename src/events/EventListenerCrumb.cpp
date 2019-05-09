#include "EventListenerCrumb.h"
#include "EventManager.h"
#include "Engine.h"

EventListenerCrumb::EventListenerCrumb(SDL_EventType eventType, EventListener* listener) : m_eventTypes{eventType}, m_listener{listener}
{

}

EventListenerCrumb::EventListenerCrumb() : EventListenerCrumb{SDL_FIRSTEVENT, nullptr}
{

}

void EventListenerCrumb::unregister()
{
    for (auto eventType : m_eventTypes)
        Engine::eventManager.removeListenerFor(eventType, m_listener);
}

void EventListenerCrumb::addEvent(SDL_EventType eventType)
{
    m_eventTypes.push_back(eventType);
}

EventListener* EventListenerCrumb::getListener()
{
    return m_listener;
}

EventListenerCrumb::~EventListenerCrumb()
{
    unregister();
}
