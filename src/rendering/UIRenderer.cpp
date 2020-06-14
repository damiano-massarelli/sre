#include "UIRenderer.h"

#include "Engine.h"
#include "materials/Shader.h"

#include <SDL.h>

void UIRenderer::init()
{
    mWindow = Engine::renderSys.getWindow();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(mWindow, SDL_GL_GetCurrentContext());
    ImGui_ImplOpenGL3_Init(Shader::GLSL_VERSION_STRING);

    Engine::eventManager.addListenerFor(EventManager::ALL_EVENTS, this, false);
}

void UIRenderer::addUIDrawer(std::function<void()> drawer)
{
    mUIDrawers.push_back(drawer);
}

void UIRenderer::setDebugUIDrawer(std::function<void()> drawer)
{
    mDebugUIDrawer = drawer;
}

void UIRenderer::onEvent(SDL_Event e)
{
    ImGui_ImplSDL2_ProcessEvent(&e);
}

void UIRenderer::render(bool showDebugUI)
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(mWindow);
    ImGui::NewFrame();

    if (showDebugUI && mDebugUIDrawer!= nullptr) {
        mDebugUIDrawer();
    }

    for (auto& drawer : mUIDrawers) {
        drawer();
    }

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIRenderer::cleanUp() {
    mUIDrawers.clear();
}

void UIRenderer::shutdown()
{
    cleanUp();
    
    // Unset the debug UI drawer
    mDebugUIDrawer = std::function<void()>();

    Engine::eventManager.removeListenerFor(EventManager::ALL_EVENTS, this);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}
