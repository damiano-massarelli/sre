#pragma once

#include "events/EventListener.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>

#include <functional>


class UIRenderer : public EventListener
{
public:
    UIRenderer() = default;

    void init();
    void render();
    void cleanUp();
    void shutdown();
    
    void setDebugUIDrawer(std::function<void()> drawer);

    // EventListener implementations
    virtual void onEvent(SDL_Event e) override;

private:
    bool mIsDebugUI = false;
    struct SDL_Window* mWindow;
    
    std::function<void()> mDebugUIDrawer = nullptr;
    
    void initDebugUI();
    void renderDebugUI();
};