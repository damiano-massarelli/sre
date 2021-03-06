#include "UIRenderer.h"

#include "Engine.h"
#include "materials/Shader.h"

#include <SDL.h>

void UIRenderer::init()
{
    mWindow = Engine::renderSys.getWindow();
}

void UIRenderer::setDebugUIDrawer(std::function<void()> drawer)
{
    if (!mIsDebugUI) {
        initDebugUI();
        mIsDebugUI = true;
    }

    mDebugUIDrawer = drawer;
}

void UIRenderer::onEvent(SDL_Event e)
{
    if (mIsDebugUI) {
        ImGui_ImplSDL2_ProcessEvent(&e);
    }
}

void UIRenderer::render()
{
    if (mIsDebugUI) {
        renderDebugUI();
    }
}

void UIRenderer::cleanUp()
{
    if (mIsDebugUI) {
        Engine::eventManager.removeListenerFor(EventManager::ALL_EVENTS, this);

        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }
}

void UIRenderer::initDebugUI()
{
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

void UIRenderer::renderDebugUI()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(mWindow);
    ImGui::NewFrame();

    if (mDebugUIDrawer) {
        mDebugUIDrawer();
    }

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
