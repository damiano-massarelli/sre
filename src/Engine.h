#ifndef ENGINE_H
#define ENGINE_H
#include "RenderSystem.h"
#include "EventManager.h"
#include "EventListener.h"
#include "GameObjectManager.h"
#include "GameObjectRenderer.h"
#include "SDL.h"
#include <cstdint>
#include <memory>

class Engine : public EventListener
{
    private:
        static std::unique_ptr<Engine> instance;
        static bool shouldQuit;

        virtual void onEvent(SDL_Event e) override;

    public:
        Engine();

        /** Manager for GameObject%s.
          * Responsible for adding and removing GameObject%s */
        static GameObjectManager gameObjectManager;

		/** Manages all the events and EventListener%s */
		static EventManager eventManager;

        /** Master Render System.
          * Manages all the global rendering settings */
        static RenderSystem renderSys;

        /** Renderer for GameObject%s */
        static GameObjectRenderer gameObjectRenderer;

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
