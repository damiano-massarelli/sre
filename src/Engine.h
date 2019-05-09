#ifndef ENGINE_H
#define ENGINE_H
#include "RenderSystem.h"
#include "EventManager.h"
#include "EventListener.h"
#include "SDL.h"
#include <cstdint>
#include <memory>

class Engine : public EventListener
{
    private:
        static std::unique_ptr<Engine> instance;
        static bool shouldQuit;

        // crumb for the quit event
        CrumbPtr mCrumb;
        virtual void onEvent(SDL_Event e) override;

    public:
        Engine();

        static RenderSystem renderSys;
        static EventManager eventManager;

        /**
          * Initializes the engine.
          * This method should be called before any other engine method */
        static void init();

        /**
          * Starts the engine
          * All systems (rendering, events, etc will start operate on a frame by frame basis
          * This method should be called after initialization is done @sa init */
        static void start();

        virtual ~Engine();
};

#endif // ENGINE_H
