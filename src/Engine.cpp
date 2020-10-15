#include "Engine.h"
#include "timer/Timer.h"
#include <SDL.h>
#include <iostream>

// Static member declarations
std::unique_ptr<Engine> Engine::instance;
bool Engine::shouldQuit = false;
EventManager Engine::eventManager;
GameObjectManager Engine::gameObjectManager;
RenderSystem Engine::renderSys;
GameObjectRenderer Engine::gameObjectRenderer;
ParticleRenderer Engine::particleRenderer;
UIRenderer Engine::uiRenderer;

Engine::Engine() { eventManager.addListenerFor(SDL_QUIT, this, false); }

void Engine::onEvent(SDL_Event e) {
    if (e.type == SDL_QUIT)
        shouldQuit = true;
}

void Engine::init() {
    if (instance == nullptr)
        instance = std::make_unique<Engine>();
}

unsigned int frames = 0;
float totDeltas;

void Engine::start() {
    if (instance == nullptr) {
        std::cerr << "Use of uninitialized engine, use init()";
        return;
    }
    Timer actualTime;
    actualTime.start();
    float elapsedMillis;

    while (!shouldQuit) {
        elapsedMillis = actualTime.getMillis();

        totDeltas += elapsedMillis;
        frames++;

        actualTime.stop();
        actualTime.start();

        eventManager.pushEnterFrameEvent(&elapsedMillis);
        eventManager.dispatchEvents();

        eventManager.pushPreRenderEvent(&elapsedMillis);
        renderSys.renderScene();
        eventManager.pushExitFrameEvent(&elapsedMillis);
    }

    shutdown();

    std::cout << totDeltas / frames << "\n";
}

void Engine::cleanUp() {
    renderSys.cleanUp();
    gameObjectManager.cleanUp();
    particleRenderer.cleanUp();
    uiRenderer.cleanUp();

    renderSys.setDefaultCamera();
}

void Engine::shutdown() {
    renderSys.shutdown();
    gameObjectManager.shutdown();
    particleRenderer.shutdown();
    uiRenderer.shutdown();
}

Engine::~Engine() { }
