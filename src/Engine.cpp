#include "Engine.h"
#include "Timer.h"
#include <SDL.h>
#include <iostream>

// Static member declarations:
std::unique_ptr<Engine> Engine::instance;
bool Engine::shouldQuit = false;
GameObjectManager Engine::gameObjectManager;
RenderSystem Engine::renderSys;
GameObjectRenderer Engine::gameObjectRenderer;
EventManager Engine::eventManager;

Engine::Engine()
{
    mCrumb = eventManager.addListenerFor(SDL_QUIT, this, true);
}

void Engine::onEvent(SDL_Event e)
{
    if (e.type == SDL_QUIT)
        shouldQuit = true;
}

void Engine::init()
{
    if (instance == nullptr)
        instance = std::make_unique<Engine>();
}

void Engine::start()
{
    if (instance == nullptr) {
        std::cerr << "Use of uninitialized engine, use init()";
        return;
    }
    Timer actualTime;
    actualTime.start();
    float elapsedSec;

    while (!shouldQuit) {
        elapsedSec = actualTime.getSeconds();
        actualTime.stop();
        actualTime.start();

        eventManager.pushEnterFrameEvent(&elapsedSec);
        eventManager.dispatchEvents();

        renderSys.prepareRendering();

        gameObjectManager.update();

        renderSys.finalizeRendering();
    }
}

Engine::~Engine()
{

}
